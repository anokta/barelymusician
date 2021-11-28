#ifndef BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_

#include <optional>
#include <unordered_map>

namespace barely {

/// Parameter definition.
struct ParamDefinition {
  /// Parameter default value.
  float default_value;

  /// Parameter minimum value.
  std::optional<float> min_value;

  /// Parameter maximum value.
  std::optional<float> max_value;
};

/// Parameter definitions container type.
using ParamDefinitions = std::unordered_map<int, ParamDefinition>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
