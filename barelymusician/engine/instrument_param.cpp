#include "barelymusician/engine/instrument_param.h"

#include <algorithm>
#include <utility>

#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

InstrumentParam::InstrumentParam(InstrumentParamDefinition definition)
    : definition_(std::move(definition)) {
  // Make sure that the default value is within the minimum and maximum values.
  SetValue(definition_.default_value);
  definition_.default_value = value_;
}

const InstrumentParamDefinition& InstrumentParam::GetDefinition() const {
  return definition_;
}

int InstrumentParam::GetId() const { return definition_.id; }

float InstrumentParam::GetValue() const { return value_; }

bool InstrumentParam::ResetValue() {
  if (value_ != definition_.default_value) {
    value_ = definition_.default_value;
    return true;
  }
  return false;
}

bool InstrumentParam::SetValue(float value) {
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
