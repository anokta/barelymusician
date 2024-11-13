#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "barelymusician.h"
#include "dsp/envelope.h"
#include "dsp/one_pole_filter.h"
#include "dsp/oscillator.h"
#include "dsp/sample_player.h"
#include "dsp/voice.h"

namespace barely::internal {

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Returns the next output sample.
  ///
  /// @tparam kFilterType Filter type.
  /// @tparam kOscillatorShape Oscillator shape.
  /// @tparam kSamplePlaybackMode Sample playback mode.
  /// @param voice Voice.
  /// @param filter_coefficient Filter coefficient.
  /// @return Next output value.
  template <FilterType kFilterType, OscillatorShape kOscillatorShape,
            SamplePlaybackMode kSamplePlaybackMode>
  [[nodiscard]] static double Next(Voice& voice, double filter_coefficient) noexcept {
    return voice.Next<kFilterType, kOscillatorShape, kSamplePlaybackMode>(filter_coefficient);
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
  void Start(const Envelope::Adsr& adsr, double intensity) noexcept {
    if (intensity > 0.0) {
      gain_ = intensity;
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

  void set_oscillator_increment(double pitch, double reference_frequency,
                                double sample_interval) noexcept {
    oscillator_.SetIncrement(pitch, reference_frequency, sample_interval);
  }
  void set_sample_player_increment(double pitch, double sample_interval) noexcept {
    sample_player_.SetIncrement(pitch, sample_interval);
  }
  void set_sample_player_slice(const SampleDataSlice* sample_player_slice) noexcept {
    sample_player_.SetSlice(sample_player_slice);
  }

 private:
  template <FilterType kFilterType, OscillatorShape kOscillatorShape,
            SamplePlaybackMode kSamplePlaybackMode>
  [[nodiscard]] double Next(double filter_coefficient) noexcept {
    if constexpr (kSamplePlaybackMode == SamplePlaybackMode::kOnce) {
      if (!sample_player_.IsActive()) {
        envelope_.Reset();
        return 0.0;
      }
    }
    const double output =
        gain_ * envelope_.Next() *
        (oscillator_.Next<kOscillatorShape>() + sample_player_.Next<kSamplePlaybackMode>());
    return filter_.Next<kFilterType>(output, filter_coefficient);
  }

  double gain_ = 0.0;
  Envelope envelope_;
  OnePoleFilter filter_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;
};

/// Voice callback signature alias.
///
/// @param voice Mutable voice.
/// @param filter_coefficient Filter coefficient.
/// @return Processed output value.
using VoiceCallback = double (*)(Voice& voice, double filter_coefficient);

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
