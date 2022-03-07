#include "barelymusician/instrument/parameter.h"

#include <algorithm>
#include <cassert>
#include <utility>

namespace barelyapi {

Parameter::Parameter(BarelyParameterDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {
  assert(value_ >= definition.min_value);
  assert(value_ <= definition.max_value);
}

const BarelyParameterDefinition& Parameter::GetDefinition() const noexcept {
  return definition_;
}

double Parameter::GetValue() const noexcept { return value_; }

bool Parameter::ResetValue() noexcept {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Parameter::SetValue(double value) noexcept {
  value =
      std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

}  // namespace barelyapi
