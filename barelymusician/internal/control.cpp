
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <utility>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Control::Control(ControlDefinition definition, SetValueCallback set_value_callback) noexcept
    : definition_(definition),
      set_value_callback_(std::move(set_value_callback)),
      value_(definition_.default_value) {
  assert(definition.default_value >= definition.min_value &&
         definition.default_value <= definition.max_value);
  assert(set_value_callback_);
}

double Control::GetValue() const noexcept { return value_; }

void Control::ResetValue() noexcept {
  if (value_ != definition_.default_value) {
    set_value_callback_(definition_.id, definition_.default_value);
    value_ = definition_.default_value;
  }
}

void Control::SetValue(double value) noexcept {
  value = std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    set_value_callback_(definition_.id, value);
    value_ = value;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::unordered_map<int, Control> BuildControls(
    const ControlDefinition* definitions, int definition_count,
    Control::SetValueCallback set_value_callback) noexcept {
  std::unordered_map<int, Control> controls;
  controls.reserve(definition_count);
  for (int index = 0; index < definition_count; ++index) {
    controls.emplace(definitions[index].id, Control(definitions[index], set_value_callback));
  }
  return controls;
}

}  // namespace barely::internal
