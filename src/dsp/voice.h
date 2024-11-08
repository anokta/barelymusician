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

struct VoiceData {
  VoiceData(int sample_rate) noexcept : adsr(sample_rate) {}

  Envelope::Adsr adsr;
  double filter_coefficient = 1.0;
  FilterType filter_type = FilterType::kNone;
  OscillatorShape oscillator_shape = OscillatorShape::kNone;
  SamplePlaybackMode sample_playback_mode = SamplePlaybackMode::kNone;
};

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Constructs a new `Voice` with the given `sample_rate`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param adsr Adsr.
  Voice(int sample_rate, const VoiceData& voice_data) noexcept;

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
  [[nodiscard]] const OnePoleFilter& filter() const noexcept { return filter_; }
  [[nodiscard]] OnePoleFilter& filter() noexcept { return filter_; }

  [[nodiscard]] const Oscillator& oscillator() const noexcept { return oscillator_; }
  [[nodiscard]] Oscillator& oscillator() noexcept { return oscillator_; }

  [[nodiscard]] const SamplePlayer& sample_player() const noexcept { return sample_player_; }
  [[nodiscard]] SamplePlayer& sample_player() noexcept { return sample_player_; }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

 private:
  Envelope envelope_;
  OnePoleFilter filter_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;
  const VoiceData& voice_data_;

  double gain_ = 0.0;
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
