#ifndef BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_
#define BARELYMUSICIAN_DSP_BIQUAD_FILTER_H_

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>
#include <utility>

#include "barelymusician.h"

namespace barely {

/// Biquad filter with smooth coefficient interpolation.
class BiquadFilter {
 public:
  /// Normalized coefficients.
  struct Coefficients {
    float a1 = 0.0f;
    float a2 = 0.0f;
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
  };

  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @param coeffs Filter coefficients.
  /// @return Filtered output sample.
  [[nodiscard]] float Next(float input, const Coefficients& coeffs) noexcept {
    // Interpolate the coefficients.
    coeffs_.a1 += (coeffs.a1 - coeffs_.a1) * kInterpolationCoefficient;
    coeffs_.a2 += (coeffs.a2 - coeffs_.a2) * kInterpolationCoefficient;
    coeffs_.b0 += (coeffs.b0 - coeffs_.b0) * kInterpolationCoefficient;
    coeffs_.b1 += (coeffs.b1 - coeffs_.b1) * kInterpolationCoefficient;
    coeffs_.b2 += (coeffs.b2 - coeffs_.b2) * kInterpolationCoefficient;

    // Uses Direct-Form 2.
    const float v0 = input - coeffs_.a1 * state_.v1 - coeffs_.a2 * state_.v2;
    const float output = coeffs_.b0 * v0 + coeffs_.b1 * state_.v1 + coeffs_.b2 * state_.v2;

    state_.v2 = state_.v1;
    state_.v1 = v0;

    return output;
  }

  /// Resets the filter.
  void Reset() noexcept { state_ = {}; }

 private:
  // Filter coefficient interpolation coefficient.
  // TODO(#146): Instead of keeping this value this low, it's probably better to add smoothening to
  // the cutoff frequency and q.
  static inline constexpr float kInterpolationCoefficient = 0.001f;

  // Filter state.
  struct State {
    float v1 = 0.0f;
    float v2 = 0.0f;
  };

  // Current coefficients.
  Coefficients coeffs_ = {};

  // Current state.
  State state_ = {};
};

/// Returns the corresponding biquad filter coefficients for a given set of filter parameters.
///
/// @param sample_interval Sampling interval in seconds.
/// @param type Filter type.
/// @param cutoff_frequency Cutoff frequency.
/// @param q Resonance quality factor.
/// @return Biquad filter coefficients.
inline BiquadFilter::Coefficients GetFilterCoefficients(float sample_interval, FilterType type,
                                                        float cutoff_frequency, float q) noexcept {
  if (type == FilterType::kNone) {
    return {};
  }

  assert(sample_interval >= 0.0f);
  assert(cutoff_frequency >= 0.0f);
  assert(q > 0.0f);

  const float w0 =
      2.0f * std::numbers::pi_v<float> * std::min(0.5f, cutoff_frequency * sample_interval);
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
