#include "barelymusician/engine/control.h"

#include <algorithm>

namespace barely::internal {

Control::Control(Definition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {}

const Control::Definition& Control::GetDefinition() const noexcept {
  return definition_;
}

double Control::GetValue() const noexcept { return value_; }

bool Control::ResetValue() noexcept {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Control::SetValue(double value) noexcept {
  value =
      std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

}  // namespace barely::internal
