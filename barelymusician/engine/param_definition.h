#ifndef BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_

#include <limits>

namespace barelyapi {

/// Parameter definition.
struct ParamDefinition {
  /// Parameter default value.
  float default_value = 0.0f;

  /// Parameter minimum value.
  float min_value = std::numeric_limits<float>::lowest();

  /// Parameter maximum value.
  float max_value = std::numeric_limits<float>::max();
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
