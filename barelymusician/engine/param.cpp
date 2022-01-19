#include "barelymusician/engine/param.h"

#include <algorithm>
#include <utility>

#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

Param::Param(ParamDefinition definition) noexcept
    : definition_(std::move(definition)), value_(definition_.default_value) {}

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
  value =
      std::min(std::max(value, definition_.min_value), definition_.max_value);
  if (value_ != value) {
    value_ = value;
    return true;
  }
  return false;
}

}  // namespace barelyapi
