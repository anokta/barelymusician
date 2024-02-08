#ifndef BARELYMUSICIAN_INTERNAL_CONTROL_H_
#define BARELYMUSICIAN_INTERNAL_CONTROL_H_

#include <functional>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

/// Class that wraps a control.
class Control {
 public:
  /// Event alias.
  using Event = ::barely::internal::Event<ControlEventDefinition, int, double>;

  /// Constructs a new `Control`.
  ///
  /// @param definition Control definition.
  explicit Control(ControlDefinition definition) noexcept;

  /// Returns the definition.
  ///
  /// @return Control definition.
  [[nodiscard]] const ControlDefinition& GetDefinition() const noexcept;

  /// Returns the slope per beat.
  ///
  /// @return Control slope in value change per second.
  [[nodiscard]] double GetSlopePerSecond() const noexcept;

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
  /// @param slope_per_second Control slope in value change per second.
  /// @return True if changed, false otherwise.
  bool Set(double value, double slope_per_second) noexcept;

  /// Updates the value by elapsed seconds.
  ///
  /// @param elapsed_seconds Elapsed seconds.
  /// @return True if changed, false otherwise.
  bool Update(double elapsed_seconds) noexcept;

 private:
  // Clamps a given `value`.
  [[nodiscard]] double Clamp(double value) noexcept;

  // Definition.
  ControlDefinition definition_;

  // Value.
  double value_ = 0.0;

  // Slope in value change per second.
  double slope_per_second_ = 0.0;
};

/// Builds the corresponding controls for a given array of control `definitions`.
///
/// @param definitions Array of control definitions.
/// @param definition_count Number of control definitions.
/// @return Array of controls.
// NOLINTNEXTLINE(bugprone-exception-escape)
std::vector<Control> BuildControls(const ControlDefinition* definitions,
                                   int definition_count) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_CONTROL_H_
