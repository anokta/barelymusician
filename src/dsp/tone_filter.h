#ifndef BARELYMUSICIAN_DSP_TONE_FILTER_H_
#define BARELYMUSICIAN_DSP_TONE_FILTER_H_

#include <algorithm>
#include <cmath>
#include <numbers>

#include "core/control.h"

namespace barely {

struct ToneFilterParams {
  double g = 0.0;
  double k = 1.0;
  double tilt_amount = 0.0;
  double tilt_coeff = 0.0;

  ToneFilterParams() noexcept { SetResonance(0.5); }

  void Approach(const ToneFilterParams& params, double coeff) noexcept {
    ApproachValue(g, params.g, coeff);
    ApproachValue(k, params.k, coeff);
    ApproachValue(tilt_amount, params.tilt_amount, coeff);
    ApproachValue(tilt_coeff, params.tilt_coeff, coeff);
  }

  void SetCutoff(double sample_rate, double cutoff) noexcept {
    const double max_freq = 0.49 * sample_rate;
    const double freq = GetFrequency(cutoff, max_freq);
    g = std::tan(std::numbers::pi_v<double> * freq / sample_rate);

    static const double kLogBaseTiltFreq = std::log(1800.0);
    static constexpr double kTiltFollowAmount = 0.5;

    const double log_freq = std::log(freq);
    const double tilt_freq =
        std::clamp(std::exp(kLogBaseTiltFreq + kTiltFollowAmount * (log_freq - kLogBaseTiltFreq)),
                   kMinFilterFreq, max_freq);
    const double tilt_g = std::tan(std::numbers::pi_v<double> * tilt_freq / sample_rate);
    tilt_coeff = tilt_g / (1.0 + tilt_g);
  }

  void SetResonance(double resonance) noexcept {
    static constexpr double kMinQ = 0.05;
    static constexpr double kMinQInverse = 1.0 / kMinQ;
    static constexpr double kMaxQ = 10.0;
    k = 1.0 / std::min(kMinQ * std::pow(kMaxQ * kMinQInverse, resonance), kMaxQ);
  }

  void SetTone(double tone) noexcept {
    static constexpr double kTiltStrength = 0.6;
    tilt_amount = tone * kTiltStrength;
  }
};

// State-variable filter with tone adjustment.
class ToneFilter {
 public:
  double Next(double input, const ToneFilterParams& params) noexcept {
    // SVF.
    const double a = 1.0 / (1.0 + params.g * (params.g + params.k));
    const double v1 = a * (s1_ + params.g * (input - s2_));
    const double v2 = s2_ + params.g * v1;

    s1_ = 2.0 * v1 - s1_;
    s2_ = 2.0 * v2 - s2_;

    // One-pole tilt.
    tilt_output_ += params.tilt_coeff * (v2 - tilt_output_);
    const double tilt_output_high = v2 - tilt_output_;

    return v2 + params.tilt_amount * (tilt_output_high - tilt_output_);
  }

  void Reset() noexcept {
    s1_ = 0.0;
    s2_ = 0.0;
    tilt_output_ = 0.0;
  }

 private:
  double s1_ = 0.0;
  double s2_ = 0.0;
  double tilt_output_ = 0.0;
};

}  // namespace barely

#endif
