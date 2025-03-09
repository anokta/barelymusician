#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "barelymusician.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "dsp/sample_generators.h"
#include "dsp/voice.h"
#include "private/random_impl.h"

namespace barely {

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Process parameters.
  // TODO(#146): Split voice parameters out of instrument parameters.
  struct Params {
    /// Bit crusher range (for bit depth reduction).
    float bit_crusher_range = 0.0f;

    /// Bit crusher increment (for sample rate reduction).
    float bit_crusher_increment = 1.0f;

    /// Filter coefficients.
    BiquadFilter::Coefficients filter_coefficients = {};

    /// Oscillator increment per sample.
    float osc_increment = 0.0f;

    /// Oscillator mix.
    float osc_mix = 0.0f;

    /// Oscillator noise ratio.
    float osc_noise_ratio = 0.0f;

    /// Oscillator shape.
    float osc_shape = 0.0f;

    /// Oscillator skew.
    float osc_skew = 0.0f;

    /// Slice increment per sample.
    float slice_increment = 0.0f;
  };

  /// Returns the next output sample.
  ///
  /// @tparam kOscMode Oscillator mode.
  /// @tparam kSliceMode Slice mode.
  /// @param voice Voice.
  /// @param params Voice process parameters.
  /// @return Next output value.
  template <OscMode kOscMode, SliceMode kSliceMode>
  [[nodiscard]] static float Next(Voice& voice, const Params& params) noexcept {
    return voice.Next<kOscMode, kSliceMode>(params);
  }

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Resets the voice.
  void Reset() noexcept { return envelope_.Reset(); }

  /// Starts the voice.
  ///
  /// @param params Voice process parameters.
  /// @param adsr Adsr.
  /// @param intensity Note intensity.
  void Start(const Params& params, const Envelope::Adsr& adsr, float intensity) noexcept {
    if (intensity > 0.0f) {
      params_ = params;
      gain_ = intensity;
      bit_crusher_.Reset();
      filter_.Reset();
      osc_phase_ = 0.0f;
      slice_offset_ = 0.0f;
      envelope_.Start(adsr);
    }
  }

  /// Stops the voice.
  template <bool IsSliceModeOnce>
  void Stop() noexcept {
    if constexpr (!IsSliceModeOnce) {
      envelope_.Stop();
    } else if (!IsSliceActive()) {
      envelope_.Reset();
    }
  }

  void set_pitch(float pitch) noexcept {
    params_.osc_increment = std::pow(2.0f, pitch);
    params_.slice_increment = (slice_ != nullptr)
                                  ? slice_->sample_rate * std::pow(2.0f, pitch - slice_->root_pitch)
                                  : 0.0f;
  }
  void set_slice(const Slice* slice) noexcept { slice_ = slice; }

 private:
  template <OscMode kOscMode, SliceMode kSliceMode>
  [[nodiscard]] float Next(const Params& params) noexcept {
    if constexpr (kSliceMode == SliceMode::kOnce) {
      if (!IsSliceActive()) {
        envelope_.Reset();
        return 0.0f;
      }
    }

    float osc_sample = 0.0f;
    float slice_sample = 0.0f;
    float output = gain_ * envelope_.Next();

    if constexpr (kSliceMode != SliceMode::kNone) {
      slice_sample = GenerateSliceSample(*slice_, slice_offset_);
    }

    if constexpr (kOscMode == OscMode::kNone) {
      output *= slice_sample;
    } else {
      const float skewed_phase = std::min(1.0f, (1.0f + params_.osc_skew) * osc_phase_);
      const float slice_output = (1.0f - std::max(0.0f, params_.osc_mix)) * slice_sample;
      osc_sample =
          (1.0f - params_.osc_noise_ratio) * GenerateOscSample(skewed_phase, params_.osc_shape) +
          params_.osc_noise_ratio * random_.DrawUniform(-1.0f, 1.0f);

      if constexpr (kOscMode == OscMode::kMix || kOscMode == OscMode::kMf) {
        output *= (1.0f - std::max(0.0f, -params_.osc_mix)) * osc_sample + slice_output;
      } else if constexpr (kOscMode == OscMode::kFm) {
        output *= slice_sample;
      } else {
        if constexpr (kOscMode == OscMode::kAm) {
          osc_sample = std::abs(osc_sample);
        } else if constexpr (kOscMode == OscMode::kEnvelopeFollower) {
          slice_sample = std::abs(slice_sample);
        }
        output *=
            (1.0f - std::max(0.0f, -params_.osc_mix)) * osc_sample * slice_sample + slice_output;
      }
    }

    output = bit_crusher_.Next(filter_.Next(output, params_.filter_coefficients),
                               params_.bit_crusher_range, params_.bit_crusher_increment);

    float osc_increment = params.osc_increment * params_.osc_increment;
    if constexpr (kOscMode == OscMode::kMf) {
      osc_increment += slice_sample * osc_increment;
    }
    osc_phase_ += osc_increment;
    if (osc_phase_ >= 1.0f) {
      osc_phase_ -= 1.0f;
    }

    float slice_increment = params.slice_increment * params_.slice_increment;
    if constexpr (kOscMode == OscMode::kFm) {
      slice_increment += 0.5f * (params_.osc_mix + 1.0f) * osc_sample * slice_increment;
    }
    slice_offset_ += slice_increment;
    if constexpr (kSliceMode == SliceMode::kLoop) {
      if (static_cast<int>(slice_offset_) >= slice_->sample_count) {
        slice_offset_ = std::fmod(slice_offset_, static_cast<float>(slice_->sample_count));
      }
    }

    ApproachParams(params);

    return output;
  }

  void ApproachParams(const Params& params) noexcept {
    // TODO(#146): Combine this with per-voice controls.
    static constexpr float kCoeff = 0.002f;
    params_.bit_crusher_increment +=
        (params.bit_crusher_increment - params_.bit_crusher_increment) * kCoeff;
    params_.bit_crusher_range += (params.bit_crusher_range - params_.bit_crusher_range) * kCoeff;
    params_.osc_mix += (params.osc_mix - params_.osc_mix) * kCoeff;
    params_.osc_noise_ratio += (params.osc_noise_ratio - params_.osc_noise_ratio) * kCoeff;
    params_.osc_shape += (params.osc_shape - params_.osc_shape) * kCoeff;
    params_.osc_skew += (params.osc_skew - params_.osc_skew) * kCoeff;
    params_.filter_coefficients = params.filter_coefficients;
  }

  bool IsSliceActive() const noexcept {
    return static_cast<int>(slice_offset_) < slice_->sample_count;
  }

  float gain_ = 0.0f;
  BitCrusher bit_crusher_;
  Envelope envelope_;
  BiquadFilter filter_;

  Params params_ = {};

  float osc_phase_ = 0.0f;
  const Slice* slice_ = nullptr;
  float slice_offset_ = 0.0f;

  // White noise random number generator.
  inline static RandomImpl random_ = RandomImpl();
};

/// Voice callback alias.
///
/// @param voice Mutable voice.
/// @param params Voice process parameters.
/// @return Processed output value.
using VoiceCallback = float (*)(Voice& voice, const Voice::Params& params);

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
