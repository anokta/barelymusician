#include "barelymusician/dsp/one_pole_filter.h"

#include <cassert>

namespace barely {

OnePoleFilter::OnePoleFilter()
    : coefficient_(1.0f), type_(FilterType::kLowPass), output_(0.0f) {}

float OnePoleFilter::Next(float input) {
  output_ = coefficient_ * (output_ - input) + input;
  if (type_ == FilterType::kHighPass) {
    return input - output_;
  }
  return output_;
}

void OnePoleFilter::Reset() { output_ = 0.0f; }

void OnePoleFilter::SetCoefficient(float coefficient) {
  assert(coefficient >= 0.0f && coefficient <= 1.0f);
  coefficient_ = coefficient;
}

void OnePoleFilter::SetType(FilterType type) { type_ = type; }

}  // namespace barely
