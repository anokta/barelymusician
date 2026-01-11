#ifndef BARELYMUSICIAN_DSP_DELAY_FILTER_H_
#define BARELYMUSICIAN_DSP_DELAY_FILTER_H_

#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#include "core/constants.h"
#include "core/control.h"
#include "dsp/one_pole_filter.h"

namespace barely {

/// Delay parameters.
struct DelayParams {
  /// Delay mix.
  float mix = 1.0f;

  /// Number of delay frames.
  float frame_count = 0.0f;

  /// Delay feedback.
  float feedback = 0.0f;

  /// Low-pass coefficient.
  float low_pass_coeff = 0.0f;

  /// High-pass coefficient.
  float high_pass_coeff = 1.0f;

  /// Approaches parameters.
  ///
  /// @param params Delay parameters to approach to.
  void Approach(const DelayParams& params) noexcept {
    ApproachValue(mix, params.mix);
    ApproachValue(frame_count, params.frame_count);
    ApproachValue(feedback, params.feedback);
    ApproachValue(low_pass_coeff, params.low_pass_coeff);
    ApproachValue(high_pass_coeff, params.high_pass_coeff);
  }
};

/// Delay filter with smooth interpolation.
class DelayFilter {
 public:
  /// Processes the next delay frame.
  ///
  /// @param input_frame Input frame.
  /// @param output_frame Output frame.
  /// @param delay_mix Delay mix.
  /// @param delay_frame_count Number of delay frames.
  /// @param delay_feedback Delay feedback.
  void Process(float input_frame[kStereoChannelCount], float output_frame[kStereoChannelCount],
               const DelayParams& params) noexcept {
    assert(params.frame_count >= 0);
    assert(static_cast<int>(params.frame_count) <= kMaxDelayFrameCount);

    const int delay_frame_count = static_cast<int>(params.frame_count);
    const int read_frame_begin =
        (write_frame_ - delay_frame_count + kMaxDelayFrameCount) % kMaxDelayFrameCount;
    const int read_frame_end = (read_frame_begin - 1 + kMaxDelayFrameCount) % kMaxDelayFrameCount;

    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      float output_sample =
          std::lerp(delay_samples_[kStereoChannelCount * read_frame_begin + channel],
                    delay_samples_[kStereoChannelCount * read_frame_end + channel],
                    params.frame_count - static_cast<float>(delay_frame_count));
      output_sample = lpf_[channel].Next<FilterType::kLowPass>(
          hpf_[channel].Next<FilterType::kHighPass>(output_sample, params.high_pass_coeff),
          params.low_pass_coeff);
      output_frame[channel] += params.mix * output_sample;
      delay_samples_[kStereoChannelCount * write_frame_ + channel] =
          input_frame[channel] + output_sample * params.feedback;
    }

    write_frame_ = (write_frame_ + 1) % kMaxDelayFrameCount;
  }

 private:
  // Low-pass filter.
  std::array<OnePoleFilter, kStereoChannelCount> lpf_ = {};

  // High-pass filter.
  std::array<OnePoleFilter, kStereoChannelCount> hpf_ = {};

  // Array of interleaved delay samples.
  std::array<float, kStereoChannelCount * kMaxDelayFrameCount> delay_samples_ = {};

  // Write frame.
  int write_frame_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DELAY_FILTER_H_
