#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "barelymusician.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "dsp/one_pole_filter.h"
#include "dsp/oscillator.h"
#include "dsp/sample_player.h"
#include "dsp/voice.h"

namespace barely::internal {

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Process parameters.
  struct Params {
    /// Bit crusher range (for bit depth reduction).
    float bit_crusher_range = 0.0f;

    /// Bit crusher increment (for sample rate reduction).
    float bit_crusher_increment = 1.0f;

    /// Filter coefficient.
    float filter_coefficient = 1.0f;

    /// Oscillator mix.
    float oscillator_mix = 0.0f;

    /// Oscillator pulse width.
    float pulse_width = 0.5f;
  };

  /// Returns the next output sample.
  ///
  /// @tparam kFilterType Filter type.
  /// @tparam kOscillatorMode Oscillator mode.
  /// @tparam kOscillatorShape Oscillator shape.
  /// @tparam kSamplePlaybackMode Sample playback mode.
  /// @param voice Voice.
  /// @param params Voice process parameters.
  /// @return Next output value.
  template <FilterType kFilterType, OscillatorMode kOscillatorMode,
            OscillatorShape kOscillatorShape, SamplePlaybackMode kSamplePlaybackMode>
  [[nodiscard]] static float Next(Voice& voice, const Params& params) noexcept {
    return voice.Next<kFilterType, kOscillatorMode, kOscillatorShape, kSamplePlaybackMode>(params);
  }

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Resets the voice.
  void Reset() noexcept { return envelope_.Reset(); }

  /// Starts the voice.
  ///
  /// @param adsr Adsr.
  /// @param intensity Note intensity.
  void Start(const Envelope::Adsr& adsr, float intensity) noexcept {
    if (intensity > 0.0f) {
      gain_ = intensity;
      bit_crusher_.Reset();
      filter_.Reset();
      oscillator_.Reset();
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

  void set_oscillator_increment(float pitch, float reference_frequency,
                                float sample_interval) noexcept {
    oscillator_.SetIncrement(pitch, reference_frequency, sample_interval);
  }
  void set_sample_player_increment(float pitch, float sample_interval) noexcept {
    sample_player_.SetIncrement(pitch, sample_interval);
  }
  void set_sample_player_slice(const SampleDataSlice* sample_player_slice) noexcept {
    sample_player_.SetSlice(sample_player_slice);
  }

 private:
  template <FilterType kFilterType, OscillatorMode kOscillatorMode,
            OscillatorShape kOscillatorShape, SamplePlaybackMode kSamplePlaybackMode>
  [[nodiscard]] float Next(const Params& params) noexcept {
    if constexpr (kSamplePlaybackMode == SamplePlaybackMode::kOnce) {
      if (!sample_player_.IsActive()) {
        envelope_.Reset();
        return 0.0f;
      }
    }

    float oscillator_sample = oscillator_.Next<kOscillatorShape>(params.pulse_width);
    float sample_player_sample = sample_player_.Next<kSamplePlaybackMode>();
    const float sample_player_output =
        (1.0f - std::max(0.0f, params.oscillator_mix)) * sample_player_sample;
    float output = gain_ * envelope_.Next();
    if constexpr (kOscillatorMode == OscillatorMode::kMix) {
      output *= ((1.0f - std::max(0.0f, -params.oscillator_mix)) * oscillator_sample +
                 sample_player_output);
    } else {
      if constexpr (kOscillatorMode == OscillatorMode::kAm) {
        oscillator_sample = std::abs(oscillator_sample);
      } else if constexpr (kOscillatorMode == OscillatorMode::kEnvelopeFollower) {
        sample_player_sample = std::abs(sample_player_sample);
      }
      output *= ((1.0f - std::max(0.0f, -params.oscillator_mix)) * oscillator_sample *
                     sample_player_sample +
                 sample_player_output);
    }
    return bit_crusher_.Next(filter_.Next<kFilterType>(output, params.filter_coefficient),
                             params.bit_crusher_range, params.bit_crusher_increment);
  }

  float gain_ = 0.0f;
  BitCrusher bit_crusher_;
  Envelope envelope_;
  OnePoleFilter filter_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;
};

/// Voice callback alias.
///
/// @param voice Mutable voice.
/// @param params Voice process parameters.
/// @return Processed output value.
using VoiceCallback = float (*)(Voice& voice, const Voice::Params& params);

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
