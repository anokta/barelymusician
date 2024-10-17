
#include "barelymusician/internal/control.h"

#include <algorithm>
#include <cassert>
#include <span>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

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
    set_value_callback_(definition_.index, definition_.default_value);
    value_ = definition_.default_value;
  }
}

void Control::SetValue(double value) noexcept {
  value = std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    set_value_callback_(definition_.index, value);
    value_ = value;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::vector<Control> BuildControls(std::span<const ControlDefinition> definitions,
                                   const Control::SetValueCallback& set_value_callback) noexcept {
  std::vector<Control> controls;
  controls.reserve(definitions.size());
  for (const auto& definition : definitions) {
    controls.emplace_back(definition, set_value_callback);
  }
  return controls;
}

}  // namespace barely
