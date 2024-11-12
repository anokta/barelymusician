#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>

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
  template <FilterType kFilterType, OscillatorShape kOscillatorShape,
            SamplePlaybackMode kSamplePlaybackMode>
  static double ProcessVoice(Voice& voice, double filter_coefficient) noexcept {
    return voice.Next<kFilterType, kOscillatorShape, kSamplePlaybackMode>(filter_coefficient);
  }

  /// Constructs a new `Voice`.
  ///
  /// @param voice_data Voice data.
  Voice(const Envelope::Adsr& adsr) noexcept : envelope_(adsr) {}

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Resets the voice.
  void Reset() noexcept { return envelope_.Reset(); }

  /// Starts the voice.
  void Start() noexcept {
    filter_state_ = 0.0;
    oscillator_phase_ = 0.0;
    sample_player_cursor_ = 0.0;
    envelope_.Start();
  }

  /// Stops the voice.
  template <bool IsSamplePlayedOnce>
  void Stop() noexcept {
    if constexpr (!IsSamplePlayedOnce) {
      envelope_.Stop();
    } else if (!is_sample_player_active()) {
      envelope_.Reset();
    }
  }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

  void set_oscillator_increment(double oscillator_increment) noexcept {
    oscillator_increment_ = oscillator_increment;
  }

  bool is_sample_player_active() const noexcept {
    return sample_player_slice_ != nullptr &&
           static_cast<int>(sample_player_cursor_) < sample_player_slice_->sample_count;
  }
  void set_sample_player_slice(const SampleDataSlice* sample_player_slice) noexcept {
    sample_player_slice_ = sample_player_slice;
  }
  void set_sample_player_speed(double speed, double sample_interval) noexcept {
    sample_player_increment_ =
        (sample_player_slice_ != nullptr && sample_player_slice_->sample_count > 0)
            ? speed * sample_player_slice_->sample_rate * sample_interval
            : 0.0;
  }

 private:
  template <FilterType kFilterType, OscillatorShape kOscillatorShape,
            SamplePlaybackMode kSamplePlaybackMode>
  double Next(double filter_coefficient) noexcept {
    if constexpr (kSamplePlaybackMode == SamplePlaybackMode::kOnce) {
      if (!is_sample_player_active()) {
        envelope_.Reset();
        return 0.0;
      }
    }
    const double output =
        gain_ * envelope_.Next() *
        (Oscillator<kOscillatorShape>(oscillator_phase_) +
         PlaySample<kSamplePlaybackMode>(sample_player_slice_, sample_player_increment_,
                                         sample_player_cursor_));
    // Update the phasor.
    oscillator_phase_ += oscillator_increment_;
    if (oscillator_phase_ >= 1.0) {
      oscillator_phase_ -= 1.0;
    }
    return Filter<kFilterType>(output, filter_coefficient, filter_state_);
  }

  Envelope envelope_;

  double filter_state_ = 0.0;

  double gain_ = 0.0;

  double oscillator_increment_ = 0.0;
  double oscillator_phase_ = 0.0;

  const SampleDataSlice* sample_player_slice_ = nullptr;
  double sample_player_cursor_ = 0.0;
  double sample_player_increment_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
