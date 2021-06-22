#include "barelymusician/engine/instrument_param.h"

#include <algorithm>
#include <utility>

namespace barelyapi {

InstrumentParam::InstrumentParam(InstrumentParamDefinition definition)
    : definition_(std::move(definition)), value_(definition_.default_value) {}

float InstrumentParam::GetValue() const { return value_; }

void InstrumentParam::ResetValue() { value_ = definition_.default_value; }

void InstrumentParam::SetValue(float value) {
  if (definition_.max_value.has_value()) {
    value = std::min(value, *definition_.max_value);
  }
  if (definition_.min_value.has_value()) {
    value = std::max(value, *definition_.min_value);
  }
  value_ = value;
}

}  // namespace barelyapi
