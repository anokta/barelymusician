#ifndef BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_
#define BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>
#include <utility>

#include "dsp/one_pole_filter.h"

namespace barely {

// Biquad filter with smooth coefficient interpolation.
class BiquadFilter {
 public:
  // Normalized coefficients.
  struct Coeffs {
    float a1 = 0.0f;
    float a2 = 0.0f;
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
  };

  [[nodiscard]] float Next(float input, const Coeffs& coeffs) noexcept {
    // Uses Transposed Direct-Form 2.
    const float output = coeffs.b0 * input + state_.v1;
    if (!std::isfinite(output)) {
      state_ = {};
      return 0.0f;
    }
    state_.v1 = coeffs.b1 * input - coeffs.a1 * output + state_.v2;
    state_.v2 = coeffs.b2 * input - coeffs.a2 * output;
    return output;
  }

  void Reset() noexcept { state_ = {}; }

 private:
  // Filter state.
  struct State {
    float v1 = 0.0f;
    float v2 = 0.0f;
  };
  State state_ = {};
};

template <FilterType kType>
BiquadFilter::Coeffs GetFilterCoeffs(float sample_rate, float cutoff_freq, float q,
                                     [[maybe_unused]] float gain_db = 0.0f) noexcept {
  assert(sample_rate > 0.0f);
  assert(cutoff_freq >= 0.0f);
  assert(cutoff_freq <= 0.5f * sample_rate);
  assert(q > 0.0f);

  const float w0 = 2.0f * std::numbers::pi_v<float> * cutoff_freq / sample_rate;
  const float cosw0 = std::cos(w0);
  const float alpha = std::sin(w0) / (2.0f * q);

  if constexpr (kType == FilterType::kLowPass) {
    const float a0 = 1.0f + alpha;
    const float b0 = (1.0f - cosw0) / (2.0f * a0);
    return {
        .a1 = (-2.0f * cosw0) / a0,
        .a2 = (1.0f - alpha) / a0,
        .b0 = b0,
        .b1 = (1.0f - cosw0) / a0,
        .b2 = b0,
    };
  } else if constexpr (kType == FilterType::kHighPass) {
    const float a0 = 1.0f + alpha;
    const float b0 = (1.0f + cosw0) / (2.0f * a0);
    return {
        .a1 = (-2.0f * cosw0) / a0,
        .a2 = (1.0f - alpha) / a0,
        .b0 = b0,
        .b1 = (-1.0f - cosw0) / a0,
        .b2 = b0,
    };
  } else {  // kHighShelf
    const float A = std::pow(10.0f, gain_db / 40.0f);
    const float two_sqrt_a_alpha = 2.0f * std::sqrt(A) * alpha;
    const float a_plus_1 = A + 1.0f;
    const float a_minus_1 = A - 1.0f;

    const float b0 = A * (a_plus_1 + a_minus_1 * cosw0 + two_sqrt_a_alpha);
    const float b1 = -2.0f * A * (a_minus_1 + a_plus_1 * cosw0);
    const float b2 = A * (a_plus_1 + a_minus_1 * cosw0 - two_sqrt_a_alpha);
    const float a0 = a_plus_1 - a_minus_1 * cosw0 + two_sqrt_a_alpha;

    return {
        .a1 = 2.0f * (a_minus_1 - a_plus_1 * cosw0) / a0,
        .a2 = (a_plus_1 - a_minus_1 * cosw0 - two_sqrt_a_alpha) / a0,
        .b0 = b0 / a0,
        .b1 = b1 / a0,
        .b2 = b2 / a0,
    };
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_
