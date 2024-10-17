
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
#include <span>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

Control::Control(ControlDefinition definition) noexcept
    : definition_(definition), value_(definition_.default_value) {
  assert(definition.default_value >= definition.min_value &&
         definition.default_value <= definition.max_value);
}

const double& Control::GetValue() const noexcept { return value_; }

bool Control::ResetValue() noexcept {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Control::SetValue(double value) noexcept {
  value = std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::vector<Control> BuildControls(std::span<const ControlDefinition> definitions) noexcept {
  std::vector<Control> controls;
  controls.reserve(definitions.size());
  for (const auto& definition : definitions) {
    controls.emplace_back(definition);
  }
  return controls;
}

}  // namespace barely
