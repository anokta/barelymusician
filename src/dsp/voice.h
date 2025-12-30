#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cmath>

#include "common/constants.h"
#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/control.h"
#include "dsp/distortion.h"
#include "dsp/envelope.h"
#include "dsp/instrument_params.h"
#include "dsp/sample_generators.h"
#include "dsp/voice.h"
#include "dsp/voice_params.h"

namespace barely {

/// Note parameters.
struct NoteParams {
  /// Gain in linear amplitude.
  float gain = 1.0f;

  /// Oscillator increment per sample.
  float osc_increment = 0.0f;

  /// Slice increment per sample.
  float slice_increment = 0.0f;
};

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Returns whether the voice is currently on.
  ///
  /// @return True if on.
  [[nodiscard]] bool IsOn() const noexcept { return envelope_.IsOn(); }

  /// Processes the next output frame.
  ///
  /// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
  /// @param voice Voice.
  /// @param params Instrument parameters.
  /// @param rng Random number generator.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param output_frame Output frame.
  template <bool kIsSidechainSend = false>
  void Process(const InstrumentParams& params, AudioRng& rng,
               float delay_frame[kStereoChannelCount], float sidechain_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount]) noexcept {
    if constexpr (kIsSidechainSend) {
      if (params_.sidechain_send <= 0.0f) {
        return;
      }
    } else {
      if (params_.sidechain_send > 0.0f) {
        return;
      }
    }

    if (params.slice_mode == SliceMode::kOnce && !IsSliceActive()) {
      envelope_.Stop();
    }

    const float skewed_osc_phase = std::min(1.0f, (1.0f + params_.osc_skew) * osc_phase_);
    const float osc_sample =
        (1.0f - params_.osc_noise_mix) * GenerateOscSample(skewed_osc_phase, params_.osc_shape) +
        params_.osc_noise_mix * rng.Generate();
    const float osc_output = params_.osc_mix * osc_sample;

    const bool has_slice = (slice_ != nullptr);
    const float slice_sample = has_slice ? GenerateSliceSample(*slice_, slice_offset_) : 0.0f;
    const float slice_output = (1.0f - params_.osc_mix) * slice_sample;

    float output = envelope_.Next();

    if (params.osc_mode == OscMode::kMix || params.osc_mode == OscMode::kMf) {
      output *= osc_output + slice_output;
    } else if (params.osc_mode == OscMode::kFm) {
      output *= slice_sample;
    } else if (params.osc_mode == OscMode::kRing) {
      output *= osc_output * slice_sample + slice_output;
    } else if (params.osc_mode == OscMode::kAm) {
      output *= std::abs(osc_output) * slice_sample + slice_output;
    } else if (params.osc_mode == OscMode::kEnvelopeFollower) {
      output *= osc_output * std::abs(slice_sample) + slice_output;
    }

    // TODO(#146): These effects should ideally be bypassed completely when they are disabled.
    output = bit_crusher_.Next(output, params_.bit_crusher_range, params_.bit_crusher_increment);
    output = Distortion(output, params_.distortion_amount, params_.distortion_drive);
    output = filter_.Next(output, params_.filter_coeffs);

    output *= params_.gain;

    float osc_increment = params.osc_increment * note_params_.osc_increment;
    if (params.osc_mode == OscMode::kMf) {
      osc_increment += slice_sample * osc_increment;
    }
    osc_phase_ += osc_increment;
    if (osc_phase_ >= 1.0f) {
      osc_phase_ -= 1.0f;
    }

    float slice_increment = params.slice_increment * note_params_.slice_increment;
    if (slice_increment > 0) {
      if (params.osc_mode == OscMode::kFm) {
        slice_increment += osc_output * slice_increment;
      }
      slice_offset_ += slice_increment;
      if (params.slice_mode == SliceMode::kLoop) {
        if (has_slice && static_cast<int>(slice_offset_) >= slice_->sample_count) {
          slice_offset_ = std::fmod(slice_offset_, static_cast<float>(slice_->sample_count));
        }
      }
    }

    const float left_gain = 0.5f * (1.0f - params_.stereo_pan);
    const float right_gain = 1.0f - left_gain;

    float left_output = left_gain * output;
    float right_output = right_gain * output;

    if constexpr (kIsSidechainSend) {
      sidechain_frame[0] += params_.sidechain_send * left_output;
      sidechain_frame[1] += params_.sidechain_send * right_output;
    } else {
      if (params_.sidechain_send < 0.0f) {
        const float sidechain_send = -params_.sidechain_send;
        left_output = std::lerp(left_output, sidechain_frame[0] * left_output, sidechain_send);
        right_output = std::lerp(right_output, sidechain_frame[1] * right_output, sidechain_send);
      }
    }

    delay_frame[0] += params_.delay_send * left_output;
    delay_frame[1] += params_.delay_send * right_output;

    output_frame[0] += left_output;
    output_frame[1] += right_output;

    Approach(params.voice_params);
  }

  /// Starts the voice.
  ///
  /// @param instrument_params Instrument parameters.
  /// @param note_controls Array of note controls.
  void Start(InstrumentParams& instrument_params, const Slice* slice, float pitch,
             const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
    const float gain = note_controls[BarelyNoteControlType_kGain];
    const float pitch_shift = note_controls[BarelyNoteControlType_kPitchShift];
    set_gain(gain);
    params_ = instrument_params.voice_params;
    params_.gain *= gain;
    pitch_ = pitch;
    pitch_shift_ = pitch_shift;
    slice_ = slice;
    UpdatePitchIncrements();
    bit_crusher_.Reset();
    filter_.Reset();
    osc_phase_ = 0.0f;
    slice_offset_ = 0.0f;
    envelope_.Start(instrument_params.adsr);
    timestamp_ = 0;
    instrument_params_ = &instrument_params;  // should be index
  }

  /// Stops the voice.
  void Stop() noexcept { envelope_.Stop(); }

  [[nodiscard]] float pitch() const noexcept { return pitch_; }
  [[nodiscard]] int timestamp() const noexcept { return timestamp_; }

  void increment_timestamp() noexcept { ++timestamp_; }
  void set_gain(float gain) noexcept { note_params_.gain = gain; }
  void set_pitch_shift(float pitch_shift) noexcept {
    pitch_shift_ = pitch_shift;
    UpdatePitchIncrements();
  }
  void set_slice(const Slice* slice) noexcept {
    slice_ = slice;
    if (slice_ != nullptr) {
      UpdatePitchIncrements();
    }
  }

  // TODO(#126): Convert this back to index.
  InstrumentParams* instrument_params_ = nullptr;

  uint32_t previous_instrument_voice_index = 0;
  uint32_t next_instrument_voice_index = 0;

 private:
  void Approach(const VoiceParams& params) noexcept {
    ApproachValue(params_.gain, note_params_.gain * params.gain);
    ApproachValue(params_.bit_crusher_increment, params.bit_crusher_increment);
    ApproachValue(params_.bit_crusher_range, params.bit_crusher_range);
    ApproachValue(params_.distortion_amount, params.distortion_amount);
    ApproachValue(params_.distortion_drive, params.distortion_drive);
    ApproachValue(params_.osc_mix, params.osc_mix);
    ApproachValue(params_.osc_noise_mix, params.osc_noise_mix);
    ApproachValue(params_.osc_shape, params.osc_shape);
    ApproachValue(params_.osc_skew, params.osc_skew);
    ApproachValue(params_.stereo_pan, params.stereo_pan);

    ApproachValue(params_.filter_coeffs.a1, params.filter_coeffs.a1);
    ApproachValue(params_.filter_coeffs.a2, params.filter_coeffs.a2);
    ApproachValue(params_.filter_coeffs.b0, params.filter_coeffs.b0);
    ApproachValue(params_.filter_coeffs.b1, params.filter_coeffs.b1);
    ApproachValue(params_.filter_coeffs.b2, params.filter_coeffs.b2);

    ApproachValue(params_.delay_send, params.delay_send);
    ApproachValue(params_.sidechain_send, params.sidechain_send);
  }

  void UpdatePitchIncrements() noexcept {
    const float pitch = pitch_ + pitch_shift_;
    note_params_.osc_increment = std::pow(2.0f, pitch);
    note_params_.slice_increment =
        (slice_ != nullptr && slice_->sample_count > 0)
            ? slice_->sample_rate * std::pow(2.0f, pitch - slice_->root_pitch)
            : 0.0f;
  }

  [[nodiscard]] bool IsSliceActive() const noexcept {
    return static_cast<int>(slice_offset_) < slice_->sample_count;
  }

  BitCrusher bit_crusher_;
  Envelope envelope_;
  BiquadFilter filter_;

  NoteParams note_params_ = {};
  VoiceParams params_ = {};

  int timestamp_ = 0;

  float pitch_ = 0.0f;
  float pitch_shift_ = 0.0f;

  float osc_phase_ = 0.0f;
  const Slice* slice_ = nullptr;
  float slice_offset_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
