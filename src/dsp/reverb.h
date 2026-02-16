#ifndef BARELYMUSICIAN_DSP_REVERB_H_
#define BARELYMUSICIAN_DSP_REVERB_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "core/constants.h"
#include "core/control.h"

namespace barely {

struct ReverbParams {
  float mix = 1.0f;
  float feedback = 0.0f;
  float damping_ratio = 0.0f;
  float width = 1.0f;
  bool freeze = false;

  void Approach(const ReverbParams& params) noexcept {
    ApproachValue(mix, params.mix);
    ApproachValue(feedback, params.feedback);
    ApproachValue(damping_ratio, params.damping_ratio);
    ApproachValue(width, params.width);
    freeze = params.freeze;
  }

  void SetFeedback(float room_size) noexcept { feedback = 0.7f + 0.28f * room_size; }
};

// Simple stereo reverb implementation based on freeverb.
class Reverb {
 public:
  void Process(float input_frame[kStereoChannelCount], float output_frame[kStereoChannelCount],
               const ReverbParams& params) noexcept {
    float damping_ratio = 0.0f;
    float feedback = 1.0f;
    float input_sample = 0.0f;
    if (!params.freeze) {
      damping_ratio = params.damping_ratio;
      feedback = params.feedback;
      input_sample = (input_frame[0] + input_frame[1]) * kStereoInputGain;
    }

    float wet_frame[kStereoChannelCount] = {};
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      for (int i = 0; i < kCombFilterCount; ++i) {
        wet_frame[channel] +=
            comb_filters_[channel][i].Process(input_sample, feedback, damping_ratio);
      }
      for (int i = 0; i < kAllPassFilterCount; ++i) {
        wet_frame[channel] = all_pass_filters_[channel][i].Process(wet_frame[channel]);
      }
    }

    const float wet_1 = params.mix * 0.5f * (1.0f + params.width);
    const float wet_2 = params.mix * 0.5f * (1.0f - params.width);
    output_frame[0] += wet_1 * wet_frame[0] + wet_2 * wet_frame[1];
    output_frame[1] += wet_1 * wet_frame[1] + wet_2 * wet_frame[0];
  }

  void SetSampleRate(float sample_rate) noexcept {
    const float sample_rate_scale = sample_rate / kTuningSampleRate;
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      for (int i = 0; i < kCombFilterCount; ++i) {
        comb_filters_[channel][i].SetFrameCount(
            GetScaledTuning(kCombFilterTunings[i], channel, sample_rate_scale));
      }
      for (int i = 0; i < kAllPassFilterCount; ++i) {
        all_pass_filters_[channel][i].SetFrameCount(
            GetScaledTuning(kAllPassFilterTunings[i], channel, sample_rate_scale));
      }
    }
  }

 private:
  static constexpr int kMaxDelayFrameCount = 8192;

  static constexpr float kTuningSampleRate = 44100.0f;

  static constexpr int kCombFilterCount = 8;
  static constexpr std::array<int, kCombFilterCount> kCombFilterTunings = {
      1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617,
  };

  static constexpr int kAllPassFilterCount = 4;
  static constexpr std::array<int, kAllPassFilterCount> kAllPassFilterTunings = {
      556,
      441,
      341,
      225,
  };
  static constexpr float kAllPassFeedback = 0.5f;

  static constexpr float kStereoInputGain = 0.0075f;
  static constexpr int kStereoSpread = 23;

  [[nodiscard]] static int GetScaledTuning(int base_tuning, int channel,
                                           float sample_rate_scale) noexcept {
    return std::clamp(static_cast<int>(static_cast<float>((base_tuning + channel * kStereoSpread)) *
                                       sample_rate_scale),
                      1, kMaxDelayFrameCount);
  }

  class CombFilter {
   public:
    [[nodiscard]] float Process(float input_sample, float feedback, float damping_ratio) noexcept {
      const float output_sample = delay_samples_[write_frame_];
      damped_sample_ = std::lerp(output_sample, damped_sample_, damping_ratio);
      delay_samples_[write_frame_] = input_sample + damped_sample_ * feedback;
      write_frame_ = (write_frame_ + 1) % frame_count_;
      return output_sample;
    }

    void SetFrameCount(int frame_count) noexcept { frame_count_ = frame_count; }

   private:
    std::array<float, kMaxDelayFrameCount> delay_samples_ = {};
    float damped_sample_ = 0.0f;
    int write_frame_ = 0;
    int frame_count_ = 1;
  };

  class AllPassFilter {
   public:
    [[nodiscard]] float Process(float input_sample) noexcept {
      const float delayed_sample = delay_samples_[write_frame_];
      const float output_sample = delayed_sample - input_sample;
      delay_samples_[write_frame_] = input_sample + delayed_sample * kAllPassFeedback;
      write_frame_ = (write_frame_ + 1) % frame_count_;
      return output_sample;
    }

    void SetFrameCount(int frame_count) noexcept { frame_count_ = frame_count; }

   private:
    std::array<float, kMaxDelayFrameCount> delay_samples_ = {};
    int write_frame_ = 0;
    int frame_count_ = 1;
  };

  std::array<std::array<CombFilter, kCombFilterCount>, kStereoChannelCount> comb_filters_ = {};
  std::array<std::array<AllPassFilter, kAllPassFilterCount>, kStereoChannelCount>
      all_pass_filters_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_REVERB_H_
