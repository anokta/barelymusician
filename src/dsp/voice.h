#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cmath>

#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
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
  /// Returns the next output sample.
  ///
  /// @tparam kOscMode Oscillator mode.
  /// @tparam kSliceMode Slice mode.
  /// @param voice Voice.
  /// @param params Instrument parameters.
  /// @return Next output value.
  template <OscMode kOscMode, SliceMode kSliceMode>
  [[nodiscard]] static float Next(Voice& voice, const InstrumentParams& params) noexcept {
    return voice.Next<kOscMode, kSliceMode>(params);
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
            ? slice_->frame_rate * std::pow(2.0f, pitch - slice_->root_pitch)
            : 0.0f;
  }
  void set_slice(const Slice* slice) noexcept { slice_ = slice; }

 private:
  template <OscMode kOscMode, SliceMode kSliceMode>
  [[nodiscard]] float Next(const InstrumentParams& params) noexcept {
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

    float output = params_.gain * envelope_.Next();

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
    output = bit_crusher_.Next(filter_.Next(output, params_.filter_coefficients),
                               params_.bit_crusher_range, params_.bit_crusher_increment);

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

    Approach(params.voice_params);

    return output;
  }

  void Approach(const VoiceParams& params) noexcept {
    ApproachParam(params_.gain, note_params_.gain * params.gain);
    ApproachParam(params_.bit_crusher_increment, params.bit_crusher_increment);
    ApproachParam(params_.bit_crusher_range, params.bit_crusher_range);
    ApproachParam(params_.osc_mix, params.osc_mix);
    ApproachParam(params_.osc_noise_mix, params.osc_noise_mix);
    ApproachParam(params_.osc_shape, params.osc_shape);
    ApproachParam(params_.osc_skew, params.osc_skew);

    ApproachParam(params_.filter_coefficients.a1, params.filter_coefficients.a1);
    ApproachParam(params_.filter_coefficients.a2, params.filter_coefficients.a2);
    ApproachParam(params_.filter_coefficients.b0, params.filter_coefficients.b0);
    ApproachParam(params_.filter_coefficients.b1, params.filter_coefficients.b1);
    ApproachParam(params_.filter_coefficients.b2, params.filter_coefficients.b2);
  }

  void ApproachParam(float& current_param, float target_param) noexcept {
    static constexpr float kCoeff = 0.002f;
    current_param += (target_param - current_param) * kCoeff;
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
/// @return Processed output value.
using VoiceCallback = float (*)(Voice& voice, const InstrumentParams& params);

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
