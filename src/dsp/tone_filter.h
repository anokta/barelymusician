#ifndef BARELYMUSICIAN_DSP_TONE_FILTER_H_
#define BARELYMUSICIAN_DSP_TONE_FILTER_H_

#include <algorithm>
#include <cmath>
#include <numbers>

#include "core/control.h"

namespace barely {

struct ToneFilterParams {
  float g = 0.0f;
  float k = 1.0f;
  float tilt_amount = 0.0f;
  float tilt_coeff = 0.0f;

  ToneFilterParams() noexcept { SetResonance(0.5f); }

  void Approach(const ToneFilterParams& params) noexcept {
    ApproachValue(g, params.g);
    ApproachValue(k, params.k);
    ApproachValue(tilt_amount, params.tilt_amount);
    ApproachValue(tilt_coeff, params.tilt_coeff);
  }

  void SetCutoff(float sample_rate, float cutoff) noexcept {
    const float max_freq = 0.49f * sample_rate;
    const float freq = GetFrequency(cutoff, max_freq);
    g = std::tan(std::numbers::pi_v<float> * freq / sample_rate);

    static const float kLogBaseTiltFreq = std::log(1800.0f);
    static constexpr float kTiltFollowAmount = 0.5f;

    const float log_freq = std::log(freq);
    const float tilt_freq =
        std::clamp(std::exp(kLogBaseTiltFreq + kTiltFollowAmount * (log_freq - kLogBaseTiltFreq)),
                   kMinFilterFreq, max_freq);
    const float tilt_g = std::tan(std::numbers::pi_v<float> * tilt_freq / sample_rate);
    tilt_coeff = tilt_g / (1.0f + tilt_g);
  }

  void SetResonance(float resonance) noexcept {
    static constexpr float kMinQ = 0.05f;
    static constexpr float kMinQInverse = 1.0f / kMinQ;
    static constexpr float kMaxQ = 10.0f;
    k = 1.0f / std::min(kMinQ * std::pow(kMaxQ * kMinQInverse, resonance), kMaxQ);
  }

  void SetTone(float tone) noexcept {
    static constexpr float kTiltStrength = 0.6f;
    tilt_amount = tone * kTiltStrength;
  }
};

// State-variable filter with tone adjustment.
class ToneFilter {
 public:
  float Next(float input, const ToneFilterParams& params) noexcept {
    // SVF.
    const float a = 1.0f / (1.0f + params.g * (params.g + params.k));
    const float v1 = a * (s1_ + params.g * (input - s2_));
    const float v2 = s2_ + params.g * v1;

    s1_ = 2.0f * v1 - s1_;
    s2_ = 2.0f * v2 - s2_;

    // One-pole tilt.
    tilt_output_ += params.tilt_coeff * (v2 - tilt_output_);
    const float tilt_output_high = v2 - tilt_output_;

    return v2 + params.tilt_amount * (tilt_output_high - tilt_output_);
  }

  void Reset() noexcept {
    s1_ = 0.0f;
    s2_ = 0.0f;
    tilt_output_ = 0.0f;
  }

 private:
  float s1_ = 0.0f;
  float s2_ = 0.0f;
  float tilt_output_ = 0.0f;
};

}  // namespace barely

#endif
