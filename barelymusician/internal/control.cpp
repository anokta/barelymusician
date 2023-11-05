
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Control::Control(ControlDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {
  assert(definition.default_value >= definition.min_value &&
         definition.default_value <= definition.max_value);
}

double Control::GetSlopePerBeat() const noexcept { return slope_per_beat_; }

double Control::GetValue() const noexcept { return value_; }

bool Control::Reset() noexcept {
  if (value_ != definition_.default_value || slope_per_beat_ != 0.0) {
    value_ = definition_.default_value;
    slope_per_beat_ = 0.0;
    return true;
  }
  return false;
}

bool Control::Set(double value, double slope_per_beat) noexcept {
  value = Clamp(value);
  if (value_ != value || slope_per_beat_ != slope_per_beat) {
    value_ = value;
    slope_per_beat_ = slope_per_beat;
    return true;
  }
  return false;
}

bool Control::Update(double duration) noexcept {
  assert(duration > 0.0);
  if (slope_per_beat_ != 0.0) {
    if (const double value = Clamp(value_ + slope_per_beat_ * duration); value_ != value) {
      value_ = value;
      return true;
    }
  }
  return false;
}

double Control::Clamp(double value) noexcept {
  return std::min(std::max(value, definition_.min_value), definition_.max_value);
}

}  // namespace barely::internal
