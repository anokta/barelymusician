#include "dsp/one_pole_filter.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "barelymusician.h"

namespace barely::internal {

double OnePoleFilter::Next(double input, double coefficient, FilterType type) noexcept {
  if (type == FilterType::kNone) {
    return input;
  }
  output_ = coefficient * (output_ - input) + input;
  if (type == FilterType::kHighPass) {
    return input - output_;
  }
  return output_;
}

void OnePoleFilter::Reset() noexcept { output_ = 0.0; }

double GetFilterCoefficient(int sample_rate, double cuttoff_frequency) noexcept {
  if (const double sample_rate_double = static_cast<double>(sample_rate);
      sample_rate_double > 0.0 && cuttoff_frequency < sample_rate_double) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::clamp(
        std::exp(-2.0 * std::numbers::pi_v<double> * cuttoff_frequency / sample_rate_double), 0.0,
        1.0);
  }
  return 0.0;
}

}  // namespace barely::internal
