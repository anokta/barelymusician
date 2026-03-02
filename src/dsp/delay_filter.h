#ifndef BARELYMUSICIAN_DSP_DELAY_FILTER_H_
#define BARELYMUSICIAN_DSP_DELAY_FILTER_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#include "core/constants.h"
#include "core/control.h"
#include "dsp/one_pole_filter.h"

namespace barely {

struct DelayParams {
  float mix = 1.0f;

  float frame_count = 1.0f;
  float feedback = 0.0f;

  float lpf_coeff = 0.0f;
  float hpf_coeff = 1.0f;

  float ping_pong = 0.0f;
  float reverb_send = 0.0f;

  void Approach(const DelayParams& params) noexcept {
    ApproachValue(mix, params.mix);
    ApproachValue(frame_count, params.frame_count);
    ApproachValue(feedback, params.feedback);
    ApproachValue(lpf_coeff, params.lpf_coeff);
    ApproachValue(hpf_coeff, params.hpf_coeff);
    ApproachValue(ping_pong, params.ping_pong);
    ApproachValue(reverb_send, params.reverb_send);
  }
};

// Delay filter with smooth interpolation.
class DelayFilter {
 public:
  void Process(float input_frame[kStereoChannelCount], float reverb_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount], const DelayParams& params) noexcept {
    assert(params.frame_count > 0);
    assert(static_cast<int>(params.frame_count) <= kMaxDelayFrameCount);

    const int delay_frame_count = static_cast<int>(params.frame_count);
    const int read_frame_begin =
        (write_frame_ - delay_frame_count + kMaxDelayFrameCount) % kMaxDelayFrameCount;
    const int read_frame_end = (read_frame_begin - 1 + kMaxDelayFrameCount) % kMaxDelayFrameCount;

    float delay_frame[kStereoChannelCount];
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      delay_frame[channel] = lpf_[channel].Next<FilterType::kLowPass>(
          hpf_[channel].Next<FilterType::kHighPass>(
              std::lerp(delay_samples_[kStereoChannelCount * read_frame_begin + channel],
                        delay_samples_[kStereoChannelCount * read_frame_end + channel],
                        params.frame_count - static_cast<float>(delay_frame_count)),
              params.hpf_coeff),
          params.lpf_coeff);
    }

    const float mono_input = 0.5f * (input_frame[0] + input_frame[1]);
    const float ping_pong_frame[kStereoChannelCount] = {
        mono_input + delay_frame[1] * params.feedback,
        delay_frame[0] * params.feedback,
    };

    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      delay_samples_[kStereoChannelCount * write_frame_ + channel] =
          std::lerp(input_frame[channel] + delay_frame[channel] * params.feedback,
                    ping_pong_frame[channel], params.ping_pong);

      const float output_sample = delay_frame[channel] * params.mix;
      reverb_frame[channel] += std::min(params.reverb_send, 1.0f) * output_sample;
      output_frame[channel] +=
          ((params.reverb_send <= 1.0f) ? 1.0f : (2.0f - params.reverb_send)) * output_sample;
    }

    write_frame_ = (write_frame_ + 1) % kMaxDelayFrameCount;
  }

 private:
  std::array<OnePoleFilter, kStereoChannelCount> lpf_ = {};
  std::array<OnePoleFilter, kStereoChannelCount> hpf_ = {};

  // Array of interleaved delay samples.
  std::array<float, kStereoChannelCount * kMaxDelayFrameCount> delay_samples_ = {};
  int write_frame_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DELAY_FILTER_H_
