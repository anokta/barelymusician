#ifndef BARELYMUSICIAN_DSP_DELAY_FILTER_H_
#define BARELYMUSICIAN_DSP_DELAY_FILTER_H_

#include <cassert>
#include <cmath>
#include <vector>

#include "common/constants.h"

namespace barely {

/// Delay filter with smooth interpolation.
class DelayFilter {
 public:
  /// Constructs a new `DelayFilter`.
  ///
  /// @param max_delay_frame_count Maximum number of delay frames.
  explicit DelayFilter(int max_delay_frame_count) noexcept
      : max_delay_frame_count_(max_delay_frame_count),
        delay_samples_(kStereoChannelCount * max_delay_frame_count, 0.0f) {
    assert(max_delay_frame_count >= 0);
  }

  /// Processes the next delay frame.
  ///
  /// @param input_frame Input frame.
  /// @param output_frame Output frame.
  /// @param delay_mix Delay mix.
  /// @param delay_frame_count Number of delay frames.
  /// @param delay_feedback Delay feedback.
  void Process(float input_frame[kStereoChannelCount], float output_frame[kStereoChannelCount],
               float delay_mix, float delay_frame_count, float delay_feedback) noexcept {
    assert(delay_frame_count >= 0);
    assert(static_cast<int>(delay_frame_count) <= max_delay_frame_count_);

    const int delay_frame_count_floor = static_cast<int>(delay_frame_count);
    const int read_frame_begin =
        (write_frame_ - delay_frame_count_floor + max_delay_frame_count_) % max_delay_frame_count_;
    const int read_frame_end =
        (read_frame_begin - 1 + max_delay_frame_count_) % max_delay_frame_count_;

    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      const float output_sample =
          std::lerp(delay_samples_[kStereoChannelCount * read_frame_begin + channel],
                    delay_samples_[kStereoChannelCount * read_frame_end + channel],
                    delay_frame_count - static_cast<float>(delay_frame_count_floor));
      output_frame[channel] += delay_mix * output_sample;
      delay_samples_[kStereoChannelCount * write_frame_ + channel] =
          input_frame[channel] + output_sample * delay_feedback;
    }

    write_frame_ = (write_frame_ + 1) % max_delay_frame_count_;
  }

 private:
  // Maximum number of delay frames.
  int max_delay_frame_count_ = 0;

  // Array of interleaved delay samples.
  std::vector<float> delay_samples_;

  // Write frame.
  int write_frame_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DELAY_FILTER_H_
