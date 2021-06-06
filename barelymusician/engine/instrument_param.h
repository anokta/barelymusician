#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_

#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

/// Instrument parameter.
struct InstrumentParam {
  /// Parameter definition.
  InstrumentParamDefinition definition;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const InstrumentParam&) const = default;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
