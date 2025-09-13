#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cmath>

#include "common/restrict.h"
#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/control.h"
#include "dsp/distortion.h"
#include "dsp/envelope.h"
#include "dsp/sample_generators.h"
#include "dsp/voice.h"

namespace barely {

/// Voice parameters.
struct VoiceParams {
  /// Bit crusher range (for bit depth reduction).
  float bit_crusher_range = 0.0f;

  /// Bit crusher increment (for sample rate reduction).
  float bit_crusher_increment = 1.0f;

  /// Distortion amount.
  float distortion_amount = 0.0f;

  /// Distortion drive.
  float distortion_drive = 1.0f;

  /// Filter coefficients.
  BiquadFilter::Coefficients filter_coefficients = {};

  /// Gain in linear amplitude.
  float gain = 1.0f;

  /// Oscillator mix.
  float osc_mix = 0.0f;

  /// Oscillator noise mix.
  float osc_noise_mix = 0.0f;

  /// Oscillator shape.
  float osc_shape = 0.0f;

  /// Oscillator skew.
  float osc_skew = 0.0f;

  /// Stereo pan.
  float stereo_pan = 0.0f;

  /// Delay send.
  float delay_send = 0.0f;

  /// Sidechain send.
  float sidechain_send = 0.0f;
};

/// Instrument parameters.
struct InstrumentParams {
  /// Oscillator increment per sample.
  float osc_increment = 0.0f;

  /// Slice increment per sample.
  float slice_increment = 0.0f;

  /// Envelope adsr.
  Envelope::Adsr adsr = {};

  /// Random number generator.
  AudioRng* rng = nullptr;

  /// Voice parameters.
  VoiceParams voice_params = {};
};

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
  /// Processes the next output frame.
  ///
  /// @tparam kOscMode Oscillator mode.
  /// @tparam kSliceMode Slice mode.
  /// @param voice Voice.
  /// @param params Instrument parameters.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param is_sidechain_send Denotes whether the sidechain frame is for send or receive.
  /// @param output_frame Output frame.
  template <OscMode kOscMode, SliceMode kSliceMode>
  static void Process(Voice& voice, const InstrumentParams& params,
                      float* BARELY_RESTRICT delay_frame, float* BARELY_RESTRICT sidechain_frame,
                      bool is_sidechain_send, float* BARELY_RESTRICT output_frame) noexcept {
    voice.Process<kOscMode, kSliceMode>(params, delay_frame, sidechain_frame, is_sidechain_send,
                                        output_frame);
  }

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Returns whether the voice is currently on.
  ///
  /// @return True if on.
  [[nodiscard]] bool IsOn() const noexcept { return envelope_.IsOn(); }

  /// Resets the voice.
  void Reset() noexcept { return envelope_.Reset(); }

  /// Starts the voice.
  ///
  /// @param instrument_params Instrument parameters.
  /// @param note_controls Array of note controls.
  void Start(const InstrumentParams& instrument_params, float pitch,
             const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
    const float gain = note_controls[BarelyNoteControlType_kGain];
    const float pitch_shift = note_controls[BarelyNoteControlType_kPitchShift];
    if (gain > 0.0f) {
      set_gain(gain);
      set_pitch(pitch + pitch_shift);
      params_ = instrument_params.voice_params;
      params_.gain *= gain;
      bit_crusher_.Reset();
      filter_.Reset();
      osc_phase_ = 0.0f;
      slice_offset_ = 0.0f;
      envelope_.Start(instrument_params.adsr);
    }
  }

  /// Stops the voice.
  void Stop() noexcept { envelope_.Stop(); }

  void set_gain(float gain) noexcept { note_params_.gain = gain; }
  void set_pitch(float pitch) noexcept {
    note_params_.osc_increment = std::pow(2.0f, pitch);
    note_params_.slice_increment =
        (slice_ != nullptr && slice_->sample_count > 0)
            ? slice_->sample_rate * std::pow(2.0f, pitch - slice_->root_pitch)
            : 0.0f;
  }
  void set_slice(const Slice* slice) noexcept { slice_ = slice; }

 private:
  template <OscMode kOscMode, SliceMode kSliceMode>
  void Process(const InstrumentParams& params, float* BARELY_RESTRICT delay_frame,
               float* BARELY_RESTRICT sidechain_frame, bool is_sidechain_send,
               float* BARELY_RESTRICT output_frame) noexcept {
    if (!IsActive() || ((is_sidechain_send && params_.sidechain_send <= 0.0f) ||
                        (!is_sidechain_send && params_.sidechain_send > 0.0f))) {
      return;
    }

    if constexpr (kSliceMode == SliceMode::kOnce) {
      if (!IsSliceActive()) {
        envelope_.Stop();
      }
    }

    assert(params.rng != nullptr);
    const float skewed_osc_phase = std::min(1.0f, (1.0f + params_.osc_skew) * osc_phase_);
    const float osc_sample =
        (1.0f - params_.osc_noise_mix) * GenerateOscSample(skewed_osc_phase, params_.osc_shape) +
        params_.osc_noise_mix * params.rng->Generate();
    const float osc_output = params_.osc_mix * osc_sample;

    const bool has_slice = (slice_ != nullptr);
    const float slice_sample = has_slice ? GenerateSliceSample(*slice_, slice_offset_) : 0.0f;
    const float slice_output = (1.0f - params_.osc_mix) * slice_sample;

    float output = envelope_.Next();

    if constexpr (kOscMode == OscMode::kMix || kOscMode == OscMode::kMf) {
      output *= osc_output + slice_output;
    } else if constexpr (kOscMode == OscMode::kFm) {
      output *= slice_sample;
    } else if constexpr (kOscMode == OscMode::kAm) {
      output *= std::abs(osc_output) * slice_sample + slice_output;
    } else if constexpr (kOscMode == OscMode::kEnvelopeFollower) {
      output *= osc_output * std::abs(slice_sample) + slice_output;
    }

    // TODO(#146): These effects should ideally be bypassed completely when they are disabled.
    output = bit_crusher_.Next(output, params_.bit_crusher_range, params_.bit_crusher_increment);
    output = Distortion(output, params_.distortion_amount, params_.distortion_drive);
    output = filter_.Next(output, params_.filter_coefficients);

    output *= params_.gain;

    float osc_increment = params.osc_increment * note_params_.osc_increment;
    if constexpr (kOscMode == OscMode::kMf) {
      osc_increment += slice_sample * osc_increment;
    }
    osc_phase_ += osc_increment;
    if (osc_phase_ >= 1.0f) {
      osc_phase_ -= 1.0f;
    }

    float slice_increment = params.slice_increment * note_params_.slice_increment;
    if (slice_increment > 0) {
      if constexpr (kOscMode == OscMode::kFm) {
        slice_increment += osc_output * slice_increment;
      }
      slice_offset_ += slice_increment;
      if constexpr (kSliceMode == SliceMode::kLoop) {
        if (has_slice && static_cast<int>(slice_offset_) >= slice_->sample_count) {
          slice_offset_ = std::fmod(slice_offset_, static_cast<float>(slice_->sample_count));
        }
      }
    }

    const float left_gain = 0.5f * (1.0f - params_.stereo_pan);
    const float right_gain = 1.0f - left_gain;

    float left_output = left_gain * output;
    float right_output = right_gain * output;

    if (is_sidechain_send) {
      sidechain_frame[0] += params_.sidechain_send * left_output;
      sidechain_frame[1] += params_.sidechain_send * right_output;
    } else if (params_.sidechain_send < 0.0f) {
      const float sidechain_send = -params_.sidechain_send;
      left_output = std::lerp(left_output, sidechain_frame[0] * left_output, sidechain_send);
      right_output = std::lerp(right_output, sidechain_frame[1] * right_output, sidechain_send);
    }

    delay_frame[0] += params_.delay_send * left_output;
    delay_frame[1] += params_.delay_send * right_output;

    output_frame[0] += left_output;
    output_frame[1] += right_output;

    Approach(params.voice_params);
  }

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

    ApproachValue(params_.filter_coefficients.a1, params.filter_coefficients.a1);
    ApproachValue(params_.filter_coefficients.a2, params.filter_coefficients.a2);
    ApproachValue(params_.filter_coefficients.b0, params.filter_coefficients.b0);
    ApproachValue(params_.filter_coefficients.b1, params.filter_coefficients.b1);
    ApproachValue(params_.filter_coefficients.b2, params.filter_coefficients.b2);

    ApproachValue(params_.delay_send, params.delay_send);
    ApproachValue(params_.sidechain_send, params.sidechain_send);
  }

  bool IsSliceActive() const noexcept {
    return static_cast<int>(slice_offset_) < slice_->sample_count;
  }

  BitCrusher bit_crusher_;
  Envelope envelope_;
  BiquadFilter filter_;

  NoteParams note_params_ = {};
  VoiceParams params_ = {};

  float osc_phase_ = 0.0f;
  const Slice* slice_ = nullptr;
  float slice_offset_ = 0.0f;
};

/// Voice callback alias.
///
/// @param voice Mutable voice.
/// @param params Instrument parameters.
/// @param delay_frame Delay send frame.
/// @param sidechain_frame Sidechain send frame.
/// @param is_sidechain_send Denotes whether the sidechain frame is for send or receive.
/// @param output_frame Output frame.
using VoiceCallback = void (*)(Voice& voice, const InstrumentParams& params,
                               float* BARELY_RESTRICT delay_frame,
                               float* BARELY_RESTRICT sidechain_frame, bool is_sidechain_send,
                               float* BARELY_RESTRICT output_frame);

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
