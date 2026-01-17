#ifndef BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_
#define BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_

#include <barelymusician.h>

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>
#include <utility>

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
    // Uses Direct-Form 2.
    const float v0 = input - coeffs.a1 * state_.v1 - coeffs.a2 * state_.v2;
    const float output = coeffs.b0 * v0 + coeffs.b1 * state_.v1 + coeffs.b2 * state_.v2;

    state_.v2 = state_.v1;
    state_.v1 = v0;

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

inline BiquadFilter::Coeffs GetFilterCoefficients(float sample_rate, FilterType type,
                                                  float cutoff_frequency, float q) noexcept {
  if (type == FilterType::kNone) {
    return {};
  }

  assert(sample_rate > 0.0f);
  assert(cutoff_frequency >= 0.0f);
  assert(q > 0.0f);

  const float w0 =
      2.0f * std::numbers::pi_v<float> * std::min(0.5f, cutoff_frequency / sample_rate);
  const float cosw0 = std::cos(w0);
  const float alpha = std::sin(w0) / (2.0f * q);
  const float a0 = 1.0f + alpha;

  switch (type) {
    case FilterType::kLowPass: {
      const float b0 = (1.0f - cosw0) / (2.0f * a0);
      return {
          .a1 = (-2.0f * cosw0) / a0,
          .a2 = (1.0f - alpha) / a0,
          .b0 = b0,
          .b1 = (1.0f - cosw0) / a0,
          .b2 = b0,
      };
    }
    case FilterType::kHighPass: {
      const float b0 = (1.0f + cosw0) / (2.0f * a0);
      return {
          .a1 = (-2.0f * cosw0) / a0,
          .a2 = (1.0f - alpha) / a0,
          .b0 = b0,
          .b1 = (-1.0f - cosw0) / a0,
          .b2 = b0,
      };
    }
    default:
      assert(!"Invalid biquad filter type");
  }

  return {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_
