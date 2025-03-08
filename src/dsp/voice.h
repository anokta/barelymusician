#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "barelymusician.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "dsp/oscillator.h"
#include "dsp/sample_player.h"
#include "dsp/voice.h"
#include "private/random_impl.h"

namespace barely {

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Process parameters.
  struct Params {
    /// Bit crusher range (for bit depth reduction).
    float bit_crusher_range = 0.0f;

    /// Bit crusher increment (for sample rate reduction).
    float bit_crusher_increment = 1.0f;

    /// Filter coefficients.
    BiquadFilter::Coefficients filter_coefficients = {};

    /// Oscillator mix.
    float osc_mix = 0.0f;

    /// Oscillator noise ratio.
    float osc_noise_ratio = 0.0f;

    /// Oscillator shape.
    float osc_shape = 0.0f;

    /// Oscillator skew.
    float osc_skew = 0.0f;
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
      osc_.Reset();
      sample_player_.Reset();
      envelope_.Start(adsr);
    }
  }

  /// Stops the voice.
  template <bool kIsSamplePlayedOnce>
  void Stop() noexcept {
    if constexpr (!kIsSamplePlayedOnce) {
      envelope_.Stop();
    } else if (!sample_player_.IsActive()) {
      envelope_.Reset();
    }
  }

  void set_osc_increment(float pitch, float reference_frequency, float sample_interval) noexcept {
    osc_.SetIncrement(pitch, reference_frequency, sample_interval);
  }
  void set_sample_player_increment(float pitch, float sample_interval) noexcept {
    sample_player_.SetIncrement(pitch, sample_interval);
  }
  void set_sample_player_slice(const Slice* sample_player_slice) noexcept {
    sample_player_.SetSlice(sample_player_slice);
  }

 private:
  template <OscMode kOscMode, SliceMode kSliceMode>
  [[nodiscard]] float Next(const Params& params) noexcept {
    if constexpr (kSliceMode == SliceMode::kOnce) {
      if (!sample_player_.IsActive()) {
        envelope_.Reset();
        return 0.0f;
      }
    }

    float output = gain_ * envelope_.Next();
    float sample_player_sample = sample_player_.GetOutput<kSliceMode>();

    if constexpr (kOscMode == OscMode::kNone) {
      output *= sample_player_sample;
      sample_player_.Increment<kSliceMode>();
    } else {
      const float sample_player_output =
          (1.0f - std::max(0.0f, params_.osc_mix)) * sample_player_sample;
      float osc_sample =
          (1.0f - params_.osc_noise_ratio) * osc_.GetOutput(params_.osc_shape, params_.osc_skew) +
          params_.osc_noise_ratio * random_.DrawUniform(-1.0f, 1.0f);

      if constexpr (kOscMode == OscMode::kMix || kOscMode == OscMode::kMf) {
        output *= (1.0f - std::max(0.0f, -params_.osc_mix)) * osc_sample + sample_player_output;
      } else if constexpr (kOscMode == OscMode::kFm) {
        output *= sample_player_sample;
      } else {
        if constexpr (kOscMode == OscMode::kAm) {
          osc_sample = std::abs(osc_sample);
        } else if constexpr (kOscMode == OscMode::kEnvelopeFollower) {
          sample_player_sample = std::abs(sample_player_sample);
        }
        output *= (1.0f - std::max(0.0f, -params_.osc_mix)) * osc_sample * sample_player_sample +
                  sample_player_output;
      }
      if constexpr (kOscMode == OscMode::kFm) {
        sample_player_.Increment<kSliceMode>(0.5f * (params_.osc_mix + 1.0f) * osc_sample);
      } else {
        sample_player_.Increment<kSliceMode>();
      }
    }
    if constexpr (kOscMode == OscMode::kMf) {
      osc_.Increment(sample_player_sample);
    } else {
      osc_.Increment();
    }

    output = bit_crusher_.Next(filter_.Next(output, params_.filter_coefficients),
                               params_.bit_crusher_range, params_.bit_crusher_increment);

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

  float gain_ = 0.0f;
  BitCrusher bit_crusher_;
  Envelope envelope_;
  BiquadFilter filter_;
  Oscillator osc_;
  SamplePlayer sample_player_;

  Params params_ = {};

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
