#ifndef BARELYMUSICIAN_DSP_DELAY_FILTER_H_
#define BARELYMUSICIAN_DSP_DELAY_FILTER_H_

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <vector>

#include "core/arena.h"
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

  void Approach(const DelayParams& params, float coeff) noexcept {
    ApproachValue(mix, params.mix, coeff);
    ApproachValue(frame_count, params.frame_count, coeff);
    ApproachValue(feedback, params.feedback, coeff);
    ApproachValue(lpf_coeff, params.lpf_coeff, coeff);
    ApproachValue(hpf_coeff, params.hpf_coeff, coeff);
    ApproachValue(ping_pong, params.ping_pong, coeff);
    ApproachValue(reverb_send, params.reverb_send, coeff);
  }
};

// Delay filter with smooth interpolation.
class DelayFilter {
 public:
  DelayFilter(Arena& arena, uint32_t max_frame_count) noexcept
      : delay_samples_(
            arena.AllocArray<float>(max_frame_count * static_cast<uint32_t>(kStereoChannelCount))),
        bit_mask_(max_frame_count - 1) {
    assert(max_frame_count > 0);
    assert(std::has_single_bit(max_frame_count));
  }

  void Process(float input_frame[kStereoChannelCount], float reverb_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount], const DelayParams& params) noexcept {
    assert(params.frame_count > 0);
    assert(static_cast<uint32_t>(params.frame_count) <= bit_mask_ + 1);

    const uint32_t delay_frame_count = static_cast<uint32_t>(params.frame_count);
    const uint32_t read_frame_begin =
        (write_frame_ + bit_mask_ + 1 - delay_frame_count) & bit_mask_;
    const uint32_t read_frame_end = (read_frame_begin + bit_mask_) & bit_mask_;

    float delay_frame[kStereoChannelCount];
    for (uint32_t channel = 0; channel < kStereoChannelCount; ++channel) {
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

    for (uint32_t channel = 0; channel < kStereoChannelCount; ++channel) {
      delay_samples_[kStereoChannelCount * write_frame_ + channel] =
          std::lerp(input_frame[channel] + delay_frame[channel] * params.feedback,
                    ping_pong_frame[channel], params.ping_pong);

      const float output_sample = delay_frame[channel] * params.mix;
      reverb_frame[channel] += std::min(params.reverb_send, 1.0f) * output_sample;
      output_frame[channel] +=
          ((params.reverb_send <= 1.0f) ? 1.0f : (2.0f - params.reverb_send)) * output_sample;
    }

    write_frame_ = (write_frame_ + 1) & bit_mask_;
  }

 private:
  std::array<OnePoleFilter, kStereoChannelCount> lpf_ = {};
  std::array<OnePoleFilter, kStereoChannelCount> hpf_ = {};

  float* delay_samples_ = nullptr;  // interleaved
  uint32_t bit_mask_ = 0;
  uint32_t write_frame_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DELAY_FILTER_H_
