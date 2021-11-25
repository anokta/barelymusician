#include "barelymusician/engine/param.h"

#include <algorithm>
#include <utility>

#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

Param::Param(ParamDefinition definition) : definition_(std::move(definition)) {
  // Make sure that the default value is within the minimum and maximum values.
  SetValue(definition_.default_value);
  definition_.default_value = value_;
}

const ParamDefinition& Param::GetDefinition() const { return definition_; }

int Param::GetId() const { return definition_.id; }

float Param::GetValue() const { return value_; }

bool Param::ResetValue() {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool Param::SetValue(float value) {
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

}  // namespace barelyapi
