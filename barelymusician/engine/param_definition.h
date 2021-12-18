#ifndef BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_

#include <optional>
#include <unordered_map>
#include <utility>

namespace barely {

/// Parameter definition.
struct ParamDefinition {
  /// Constructs new |ParamDefinition|.
  ///
  /// @param default_value Default boolean value.
  explicit ParamDefinition(bool default_value)
      : default_value(static_cast<float>(default_value)),
        min_value(std::nullopt),
        max_value(std::nullopt) {}

  /// Constructs new |ParamDefinition|.
  ///
  /// @param default_value Default value.
  /// @param min_value Optional minimum value.
  /// @param max_value Optional maximum value.
  explicit ParamDefinition(float default_value = 0.0f,
                           std::optional<float> min_value = std::nullopt,
                           std::optional<float> max_value = std::nullopt)
      : default_value(default_value),
        min_value(std::move(min_value)),
        max_value(std::move(max_value)) {}

  /// Constructs new |ParamDefinition|.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Optional minimum integer value.
  /// @param max_value Optional maximum integer value.
  explicit ParamDefinition(int default_value,
                           std::optional<int> min_value = std::nullopt,
                           std::optional<int> max_value = std::nullopt)
      : default_value(static_cast<float>(default_value)),
        min_value(min_value
                      ? std::optional<float>{static_cast<float>(*min_value)}
                      : std::nullopt),
        max_value(max_value
                      ? std::optional<float>{static_cast<float>(*max_value)}
                      : std::nullopt) {}

  /// Parameter default value.
  float default_value;

  /// Parameter minimum value.
  std::optional<float> min_value;

  /// Parameter maximum value.
  std::optional<float> max_value;
};

/// Parameter definition by id map type.
using ParamDefinitionMap = std::unordered_map<int, ParamDefinition>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAM_DEFINITION_H_
