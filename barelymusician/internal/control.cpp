
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Control::Control(ControlDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {
  assert(definition.default_value >= definition.min_value &&
         definition.default_value <= definition.max_value);
}

const ControlDefinition& Control::GetDefinition() const noexcept { return definition_; }

double Control::GetValue() const noexcept { return value_; }

bool Control::Reset() noexcept {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Control::Set(double value) noexcept {
  value = Clamp(value);
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

double Control::Clamp(double value) noexcept {
  return std::min(std::max(value, definition_.min_value), definition_.max_value);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::unordered_map<int, Control> BuildControls(const ControlDefinition* definitions,
                                               int definition_count) noexcept {
  std::unordered_map<int, Control> controls;
  controls.reserve(definition_count);
  for (int index = 0; index < definition_count; ++index) {
    controls.emplace(definitions[index].id, definitions[index]);
  }
  return controls;
}

}  // namespace barely::internal
