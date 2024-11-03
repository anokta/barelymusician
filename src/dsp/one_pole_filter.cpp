#include "dsp/one_pole_filter.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "barelymusician.h"

namespace barely::internal {

double OnePoleFilter::Next(double input) noexcept {
  if (type_ == FilterType::kNone) {
    return input;
  }
  output_ = coefficient_ * (output_ - input) + input;
  if (type_ == FilterType::kHighPass) {
    return input - output_;
  }
  return output_;
}

void OnePoleFilter::Reset() noexcept { output_ = 0.0; }

void OnePoleFilter::SetCoefficient(double coefficient) noexcept {
  coefficient_ = std::min(std::max(coefficient, 0.0), 1.0);
}

void OnePoleFilter::SetType(FilterType type) noexcept { type_ = type; }

double GetFilterCoefficient(int frame_rate, double cuttoff_frequency) noexcept {
  if (const double frame_rate_double = static_cast<double>(frame_rate);
      frame_rate_double > 0.0 && cuttoff_frequency < frame_rate_double) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-2.0 * std::numbers::pi_v<double> * cuttoff_frequency / frame_rate_double);
  }
  return 0.0;
}

}  // namespace barely::internal
