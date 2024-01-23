
#include "barelymusician/internal/control.h"

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

Rational Control::GetSlopePerBeat() const noexcept { return slope_per_beat_; }

Rational Control::GetValue() const noexcept { return value_; }

bool Control::Reset() noexcept {
  if (value_ != definition_.default_value || slope_per_beat_ != 0) {
    value_ = definition_.default_value;
    slope_per_beat_ = 0;
    return true;
  }
  return false;
}

bool Control::Set(Rational value, Rational slope_per_beat) noexcept {
  value = Clamp(value);
  if (value_ != value || slope_per_beat_ != slope_per_beat) {
    value_ = value;
    slope_per_beat_ = slope_per_beat;
    return true;
  }
  return false;
}

bool Control::Update(Rational duration) noexcept {
  assert(duration > 0);
  if (slope_per_beat_ != 0) {
    if (const Rational value = Clamp(value_ + slope_per_beat_ * duration); value_ != value) {
      value_ = value;
      return true;
    }
  }
  return false;
}

Rational Control::Clamp(Rational value) noexcept {
  if (value < definition_.min_value) {
    return definition_.min_value;
  }
  if (value > definition_.max_value) {
    return definition_.max_value;
  }
  return value;
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
