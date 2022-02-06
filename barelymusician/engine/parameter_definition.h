#ifndef BARELYMUSICIAN_ENGINE_PARAMETER_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_PARAMETER_DEFINITION_H_

#include <limits>

namespace barelyapi {

/// Parameter definition.
struct ParameterDefinition {
  /// Default value.
  float default_value = 0.0f;

  /// Minimum value.
  float min_value = std::numeric_limits<float>::lowest();

  /// Maximum value.
  float max_value = std::numeric_limits<float>::max();
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PARAMETER_DEFINITION_H_
