#ifndef BARELYMUSICIAN_INTERNAL_CONTROL_H_
#define BARELYMUSICIAN_INTERNAL_CONTROL_H_

#include <functional>
#include <unordered_map>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps a control.
class Control {
 public:
  /// Default constructor.
  Control() noexcept = default;

  /// Constructs a new `Control`.
  ///
  /// @param definition Control definition.
  explicit Control(ControlDefinition definition) noexcept;

  /// Returns the value.
  ///
  /// @return Control value.
  [[nodiscard]] double GetValue() const noexcept;

  /// Resets the value.
  ///
  /// @return True if changed, false otherwise.
  bool Reset() noexcept;

  /// Sets the value.
  ///
  /// @param value Control value.
  /// @return True if changed, false otherwise.
  bool Set(double value) noexcept;

 private:
  // Clamps a given `value`.
  [[nodiscard]] double Clamp(double value) noexcept;

  // Definition.
  ControlDefinition definition_;

  // Value.
  double value_ = 0.0;
};

/// Builds the corresponding controls for a given array of control `definitions`.
///
/// @param definitions Array of control definitions.
/// @param definition_count Number of control definitions.
/// @return Array of controls.
// NOLINTNEXTLINE(bugprone-exception-escape)
std::unordered_map<int, Control> BuildControls(const ControlDefinition* definitions,
                                               int definition_count) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_CONTROL_H_
