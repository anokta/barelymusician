#ifndef BARELYMUSICIAN_API_PARAMETER_H_
#define BARELYMUSICIAN_API_PARAMETER_H_

// NOLINTBEGIN
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Parameter definition.
typedef struct BarelyParameterDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyParameterDefinition;

/// Parameter state.
typedef struct BarelyParameterState {
  /// Value.
  double value;

  /// Increment per frame.
  double increment;
} BarelyParameterState;

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <limits>

namespace barely {

/// Parameter definition.
struct ParameterDefinition : public BarelyParameterDefinition {
  /// Constructs new `ParameterDefinition`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  explicit ParameterDefinition(
      double default_value,
      double min_value = std::numeric_limits<double>::lowest(),
      double max_value = std::numeric_limits<double>::max())
      : BarelyParameterDefinition{default_value, min_value, max_value} {}

  /// Constructs new `ParameterDefinition` for a boolean value.
  ///
  /// @param default_value Default boolean value.
  explicit ParameterDefinition(bool default_value)
      : ParameterDefinition(static_cast<double>(default_value)) {}

  /// Constructs new `ParameterDefinition` for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ParameterDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max())
      : ParameterDefinition(static_cast<double>(default_value),
                            static_cast<double>(min_value),
                            static_cast<double>(max_value)) {}

  /// Constructs new `ParameterDefinition` from internal type.
  ///
  /// @param definition Internal parameter definition.
  explicit ParameterDefinition(BarelyParameterDefinition definition)
      : BarelyParameterDefinition(definition) {}
};

/// Parameter state.
struct ParameterState : public BarelyParameterState {
  /// Constructs new `ParameterState`.
  ///
  /// @param value Value.
  /// @param slope Slope.
  explicit ParameterState(double value, double slope = 0.0)
      : BarelyParameterState{value, slope} {}

  /// Constructs new `ParameterState` from internal type.
  ///
  /// @param state Internal parameter state.
  explicit ParameterState(BarelyParameterState state)
      : BarelyParameterState(state) {}
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_PARAMETER_H_
