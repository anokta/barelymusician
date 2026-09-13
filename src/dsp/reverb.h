#ifndef BARELYMUSICIAN_DSP_REVERB_H_
#define BARELYMUSICIAN_DSP_REVERB_H_

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>

#include "core/arena.h"
#include "core/constants.h"
#include "core/control.h"

namespace barely {

inline constexpr double kMaxDampingRatio = 0.4;

struct ReverbParams {
  double mix = 1.0;
  double feedback = 0.0;
  double damping_ratio = 0.0;
  double width = 1.0;
  bool freeze = false;

  void Approach(const ReverbParams& params, double coeff) noexcept {
    ApproachValue(mix, params.mix, coeff);
    ApproachValue(feedback, params.feedback, coeff);
    ApproachValue(damping_ratio, params.damping_ratio, coeff);
    ApproachValue(width, params.width, coeff);
    freeze = params.freeze;
  }

  void SetFeedback(double room_size) noexcept { feedback = 0.7 + 0.28 * room_size; }
};

// Simple stereo reverb implementation based on freeverb.
class Reverb {
 public:
  Reverb(Arena& arena, double sample_rate) noexcept {
    const double sample_rate_scale = sample_rate / kTuningSampleRate;
    const uint32_t max_delay_frame_count = std::bit_ceil(static_cast<uint32_t>(
        GetScaledTuning(kCombFilterTunings[kCombFilterCount - 1], 1, sample_rate_scale)));
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      for (int i = 0; i < kCombFilterCount; ++i) {
        comb_filters_[channel][i].Init(arena, max_delay_frame_count);
      }
      for (int i = 0; i < kAllPassFilterCount; ++i) {
        all_pass_filters_[channel][i].Init(arena, max_delay_frame_count);
      }
    }
    if (arena.is_null()) {
      return;
    }

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

  void Process(const double input_frame[kStereoChannelCount],
               double output_frame[kStereoChannelCount], const ReverbParams& params) noexcept {
    double damping_ratio = 0.0;
    double feedback = kMaxDelayFeedback;
    double input_sample = 0.0;
    if (!params.freeze) {
      damping_ratio = params.damping_ratio;
      feedback = params.feedback;
      input_sample = (input_frame[0] + input_frame[1]) * kStereoInputGain;
    }

    double wet_frame[kStereoChannelCount] = {};
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      for (int i = 0; i < kCombFilterCount; ++i) {
        wet_frame[channel] +=
            comb_filters_[channel][i].Process(input_sample, feedback, damping_ratio);
      }
      for (int i = 0; i < kAllPassFilterCount; ++i) {
        wet_frame[channel] = all_pass_filters_[channel][i].Process(wet_frame[channel]);
      }
    }

    const double wet_1 = params.mix * 0.5 * (1.0 + params.width);
    const double wet_2 = params.mix * 0.5 * (1.0 - params.width);
    output_frame[0] += wet_1 * wet_frame[0] + wet_2 * wet_frame[1];
    output_frame[1] += wet_1 * wet_frame[1] + wet_2 * wet_frame[0];
  }

 private:
  static constexpr double kTuningSampleRate = 44100.0;

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
  static constexpr double kAllPassFeedback = 0.5;

  static constexpr double kStereoInputGain = 0.0075;
  static constexpr int kStereoSpread = 23;

  [[nodiscard]] static int GetScaledTuning(int base_tuning, int channel,
                                           double sample_rate_scale) noexcept {
    return std::max(static_cast<int>(static_cast<double>((base_tuning + channel * kStereoSpread)) *
                                     sample_rate_scale),
                    1);
  }

  class CombFilter {
   public:
    void Init(Arena& arena, uint32_t max_delay_frame_count) noexcept {
      delay_samples_ = arena.AllocArray<double>(max_delay_frame_count);
    }

    [[nodiscard]] double Process(double input_sample, double feedback,
                                 double damping_ratio) noexcept {
      const double output_sample = delay_samples_[write_frame_];
      damped_sample_ = std::lerp(output_sample, damped_sample_, damping_ratio);
      delay_samples_[write_frame_] = input_sample + damped_sample_ * feedback;
      if (++write_frame_ == frame_count_) {
        write_frame_ = 0;
      }
      return output_sample;
    }

    void SetFrameCount(int frame_count) noexcept { frame_count_ = frame_count; }

   private:
    double* delay_samples_ = nullptr;
    double damped_sample_ = 0.0;
    int write_frame_ = 0;
    int frame_count_ = 1;
  };

  class AllPassFilter {
   public:
    void Init(Arena& arena, uint32_t max_delay_frame_count) noexcept {
      delay_samples_ = arena.AllocArray<double>(max_delay_frame_count);
    }

    [[nodiscard]] double Process(double input_sample) noexcept {
      const double delayed_sample = delay_samples_[write_frame_];
      const double output_sample = delayed_sample - input_sample;
      delay_samples_[write_frame_] = input_sample + delayed_sample * kAllPassFeedback;
      if (++write_frame_ == frame_count_) {
        write_frame_ = 0;
      }
      return output_sample;
    }

    void SetFrameCount(int frame_count) noexcept { frame_count_ = frame_count; }

   private:
    double* delay_samples_ = nullptr;
    int write_frame_ = 0;
    int frame_count_ = 1;
  };

  std::array<std::array<CombFilter, kCombFilterCount>, kStereoChannelCount> comb_filters_ = {};
  std::array<std::array<AllPassFilter, kAllPassFilterCount>, kStereoChannelCount>
      all_pass_filters_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_REVERB_H_
