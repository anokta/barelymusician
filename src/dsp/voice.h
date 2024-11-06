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
  /// Constructs a new `Voice` with the given `frame_rate`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Voice(int frame_rate) noexcept;

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
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  template <bool kShouldAccumulate>
  void Process(double* output_samples, int output_channel_count, int output_frame_count) noexcept;

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
void Voice::Process(double* output_samples, int output_channel_count,
                    int output_frame_count) noexcept {
  const int output_sample_count = output_channel_count * output_frame_count;
  for (int i = 0; i < output_sample_count; i += output_channel_count) {
    double* output_frame = output_samples + i;
    if (!IsActive()) {
      if constexpr (!kShouldAccumulate) {
        std::fill_n(output_frame, output_sample_count - i, 0.0);
      }
      return;
    }
    const double mono_sample = Next();
    for (int channel = 0; channel < output_channel_count; ++channel) {
      if constexpr (kShouldAccumulate) {
        output_frame[channel] += mono_sample;
      } else {
        output_frame[channel] = mono_sample;
      }
    }
  }
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
