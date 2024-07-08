#ifndef BARELYMUSICIAN_INTERNAL_CONTROL_H_
#define BARELYMUSICIAN_INTERNAL_CONTROL_H_

#include <functional>
#include <unordered_map>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps a control.
class Control {
 public:
  /// Set value callback alias.
  using SetValueCallback = std::function<void(int, double)>;

  Control() noexcept = default;

  /// Constructs a new `Control`.
  ///
  /// @param definition Control definition.
  /// @param set_value_callback Set value callback.
  explicit Control(ControlDefinition definition, SetValueCallback set_value_callback) noexcept;

  /// Returns the value.
  ///
  /// @return Control value.
  [[nodiscard]] double GetValue() const noexcept;

  /// Resets the value.
  void ResetValue() noexcept;

  /// Sets the value.
  ///
  /// @param value Control value.
  void SetValue(double value) noexcept;

 private:
  // Definition.
  ControlDefinition definition_;

  // Set value callback.
  SetValueCallback set_value_callback_;

  // Value.
  double value_ = 0.0;
};

/// Builds the corresponding controls for a given array of control `definitions`.
///
/// @param definitions Array of control definitions.
/// @param definition_count Number of control definitions.
/// @param set_value_callback Set value callback.
/// @return Array of controls.
// NOLINTNEXTLINE(bugprone-exception-escape)
std::unordered_map<int, Control> BuildControls(
    const ControlDefinition* definitions, int definition_count,
    Control::SetValueCallback set_value_callback) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_CONTROL_H_
