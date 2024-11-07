#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>

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
  /// Constructs a new `Voice` with the given `sample_rate`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Voice(int sample_rate) noexcept;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Returns the next output sample.
  ///
  /// @return Output sample.
  double Next() noexcept;

  /// Processes the next output samples.
  ///
  /// @param output_samples Array of mono output samples.
  /// @param output_sample_count Number of output samples.
  template <bool kShouldAccumulate>
  void Process(double* output_samples, int output_sample_count) noexcept;

  /// Resets the voice.
  void Reset() noexcept;

  /// Starts the voice.
  void Start() noexcept;

  /// Stops the voice.
  void Stop() noexcept;

  /// Inline getter/setter functions.
  [[nodiscard]] const Envelope& envelope() const noexcept { return envelope_; }
  [[nodiscard]] Envelope& envelope() noexcept { return envelope_; }

  [[nodiscard]] const OnePoleFilter& filter() const noexcept { return filter_; }
  [[nodiscard]] OnePoleFilter& filter() noexcept { return filter_; }

  [[nodiscard]] const Oscillator& oscillator() const noexcept { return oscillator_; }
  [[nodiscard]] Oscillator& oscillator() noexcept { return oscillator_; }

  [[nodiscard]] const SamplePlayer& sample_player() const noexcept { return sample_player_; }
  [[nodiscard]] SamplePlayer& sample_player() noexcept { return sample_player_; }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

  void set_sample_playback_mode(SamplePlaybackMode sample_playback_mode) noexcept {
    sample_playback_mode_ = sample_playback_mode;
    sample_player_.SetLoop(sample_playback_mode_ == SamplePlaybackMode::kLoop);
  }

 private:
  Envelope envelope_;
  OnePoleFilter filter_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;

  double gain_ = 0.0;
  SamplePlaybackMode sample_playback_mode_ = SamplePlaybackMode::kNone;
};

template <bool kShouldAccumulate>
void Voice::Process(double* output_samples, int output_sample_count) noexcept {
  for (int i = 0; i < output_sample_count; ++i) {
    if (!IsActive()) {
      if constexpr (!kShouldAccumulate) {
        std::fill(output_samples + i, output_samples + output_sample_count, 0.0);
      }
      return;
    }
    if constexpr (kShouldAccumulate) {
      output_samples[i] += Next();
    } else {
      output_samples[i] = Next();
    }
  }
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
