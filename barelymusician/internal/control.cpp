
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Control::Control(ControlDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {
  assert(definition.default_value >= definition.min_value &&
         definition.default_value <= definition.max_value);
}

const ControlDefinition& Control::GetDefinition() const noexcept { return definition_; }

double Control::GetSlopePerSecond() const noexcept { return slope_per_second_; }

double Control::GetValue() const noexcept { return value_; }

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

bool Control::Update(double interval) noexcept {
  assert(interval > 0.0);
  if (slope_per_second_ != 0.0) {
    if (const double value = Clamp(value_ + slope_per_second_ * interval); value_ != value) {
      value_ = value;
      return true;
    }
  }
  return false;
}

double Control::Clamp(double value) noexcept {
  return std::min(std::max(value, definition_.min_value), definition_.max_value);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::vector<Control> BuildControls(const ControlDefinition* definitions,
                                   int definition_count) noexcept {
  std::vector<Control> controls;
  controls.reserve(definition_count);
  for (int index = 0; index < definition_count; ++index) {
    controls.emplace_back(definitions[index]);
  }
  return controls;
}

}  // namespace barely::internal
