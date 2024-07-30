
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
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
    set_value_callback_(definition_.control_id, definition_.default_value);
    value_ = definition_.default_value;
  }
}

void Control::SetValue(double value) noexcept {
  value = std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    set_value_callback_(definition_.control_id, value);
    value_ = value;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
ControlMap BuildControlMap(const ControlDefinition* definitions, int definition_count,
                           Control::SetValueCallback set_value_callback) noexcept {
  ControlMap control_map;
  control_map.reserve(definition_count);
  for (int index = 0; index < definition_count; ++index) {
    control_map.emplace(definitions[index].control_id,
                        Control(definitions[index], set_value_callback));
  }
  return control_map;
}

}  // namespace barely::internal
