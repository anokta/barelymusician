#include "barelymusician/instrument/one_pole_filter.h"

#include <algorithm>

namespace barelyapi {

OnePoleFilter::OnePoleFilter() : coefficient_(1.0f), output_(0.0f) {}

float OnePoleFilter::ProcessNext(float input) {
  output_ = coefficient_ * (output_ - input) + input;
  if (type_ == Type::kHighPass) {
    return input - output_;
  }
  return output_;
}

void OnePoleFilter::SetCoefficient(float coefficient) {
  coefficient_ = std::min(std::max(coefficient, 0.0f), 1.0f);
}

void OnePoleFilter::SetType(Type type) { type_ = type; }

}  // namespace barelyapi
