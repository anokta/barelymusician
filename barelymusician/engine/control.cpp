
#include "barelymusician/engine/control.h"

#include <algorithm>
#include <cassert>

namespace barely::internal {

Control::Control(ControlDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {}

double Control::Get() const noexcept { return value_; }

bool Control::Reset() noexcept {
  if (value_ != definition_.default_value || slope_per_second_ != 0.0) {
    value_ = definition_.default_value;
    slope_per_second_ = 0.0;
    return true;
  }
  return false;
}

bool Control::Set(double value, double slope_per_second) noexcept {
  value = Clamp(value);
  if (value_ != value || slope_per_second_ != slope_per_second) {
    value_ = value;
    slope_per_second_ = slope_per_second;
    return true;
  }
  return false;
}

bool Control::UpdateBy(double elapsed_seconds) noexcept {
  assert(elapsed_seconds > 0.0);
  if (slope_per_second_ != 0.0) {
    if (const double value =
            Clamp(value_ + slope_per_second_ * elapsed_seconds);
        value_ != value) {
      value_ = value;
      return true;
    }
  }
  return false;
}

double Control::Clamp(double value) noexcept {
  return std::min(std::max(value, definition_.min_value),
                  definition_.max_value);
}

}  // namespace barely::internal
