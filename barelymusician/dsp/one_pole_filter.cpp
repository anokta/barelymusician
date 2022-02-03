#include "barelymusician/dsp/one_pole_filter.h"

#include <algorithm>

namespace barelyapi {

float OnePoleFilter::Next(float input) noexcept {
  output_ = coefficient_ * (output_ - input) + input;
  if (type_ == FilterType::kHighPass) {
    return input - output_;
  }
  return output_;
}

void OnePoleFilter::Reset() noexcept { output_ = 0.0f; }

void OnePoleFilter::SetCoefficient(float coefficient) noexcept {
  coefficient_ = std::min(std::max(coefficient, 0.0f), 1.0f);
}

void OnePoleFilter::SetType(FilterType type) noexcept { type_ = type; }

}  // namespace barelyapi
