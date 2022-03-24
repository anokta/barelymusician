#include "barelymusician/dsp/one_pole_filter.h"

#include <algorithm>

namespace barelyapi {

double OnePoleFilter::Next(double input) noexcept {
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

}  // namespace barelyapi
