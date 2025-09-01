#ifndef BARELYMUSICIAN_DSP_DELAY_H_
#define BARELYMUSICIAN_DSP_DELAY_H_

#include <cassert>
#include <cmath>
#include <vector>

namespace barely {

class Delay {
 public:
  Delay(int channel_count, int max_delay_frame_count) noexcept
      : channel_count_(channel_count),
        max_delay_frame_count_(max_delay_frame_count),
        delay_samples_(max_delay_frame_count * channel_count, 0.0f) {
    assert(channel_count > 0);
    assert(max_delay_frame_count >= 0);
  }

  void Process(const float* input_frame, float* output_frame, float delay_mix,
               float delay_frame_count, float feedback) noexcept {
    assert(delay_frame_count <= static_cast<float>(max_delay_frame_count_));

    const int delay_frame_count_int = static_cast<int>(delay_frame_count);
    const float delay_frame_count_frac =
        delay_frame_count - static_cast<float>(delay_frame_count_int);

    const int read_frame_begin =
        (write_frame_ - delay_frame_count_int + max_delay_frame_count_) % max_delay_frame_count_;
    const int read_frame_end =
        (read_frame_begin - 1 + max_delay_frame_count_) % max_delay_frame_count_;

    for (int channel = 0; channel < channel_count_; ++channel) {
      const float output_sample = std::lerp(
          delay_samples_[read_frame_begin * channel_count_ + channel],
          delay_samples_[read_frame_end * channel_count_ + channel], delay_frame_count_frac);
      output_frame[channel] += delay_mix * output_sample;
      delay_samples_[write_frame_ * channel_count_ + channel] =
          input_frame[channel] + output_sample * feedback;
    }

    write_frame_ = (write_frame_ + 1) % max_delay_frame_count_;
  }

 private:
  // Number of channels.
  int channel_count_ = 0;

  // Maximum number of delay frames.
  int max_delay_frame_count_ = 0;

  // Array of interleaved delay samples.
  std::vector<float> delay_samples_;

  // Read frame.
  int read_frame_ = 0;

  // Write frame.
  int write_frame_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DELAY_H_
