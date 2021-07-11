#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_DEFINITION_H_

#include <optional>
#include <vector>

namespace barelyapi {

/// Instrument parameter definition.
struct InstrumentParamDefinition {
  /// Parameter id.
  int id;

  /// Parameter default value.
  float default_value;

  /// Parameter minimum value.
  std::optional<float> min_value;

  /// Parameter maximum value.
  std::optional<float> max_value;
};

/// Instrument parameter definitions container type.
using InstrumentParamDefinitions = std::vector<InstrumentParamDefinition>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_DEFINITION_H_
