#include "barelymusician/engine/param.h"

#include <algorithm>
#include <utility>

#include "barelymusician/engine/param_definition.h"

namespace barely {

Param::Param(ParamDefinition definition) noexcept
    : definition_(std::move(definition)) {
  // Make sure that the default value is within the minimum and maximum values.
  SetValue(definition_.default_value);
  definition_.default_value = value_;
}

const ParamDefinition& Param::GetDefinition() const noexcept {
  return definition_;
}

float Param::GetValue() const noexcept { return value_; }

bool Param::ResetValue() noexcept {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Param::SetValue(float value) noexcept {
  if (definition_.max_value.has_value()) {
    value = std::min(value, *definition_.max_value);
  }
  if (definition_.min_value.has_value()) {
    value = std::max(value, *definition_.min_value);
  }
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

}  // namespace barely
