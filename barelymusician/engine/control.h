#ifndef BARELYMUSICIAN_ENGINE_CONTROL_H_
#define BARELYMUSICIAN_ENGINE_CONTROL_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/number.h"

namespace barely::internal {

/// Control definition alias.
using ControlDefinition = barely::ControlDefinition;

/// Class that wraps control.
class Control {
 public:
  /// Constructs new `Control`.
  ///
  /// @param definition Control definition.
  explicit Control(ControlDefinition definition) noexcept;

  /// Returns slope per beat.
  ///
  /// @return Control slope in value change per beat.
  [[nodiscard]] Real GetSlopePerBeat() const noexcept;

  /// Returns value.
  ///
  /// @return Control value.
  [[nodiscard]] Real GetValue() const noexcept;

  /// Resets value.
  ///
  /// @return True if changed, false otherwise.
  bool Reset() noexcept;

  /// Sets value.
  ///
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return True if changed, false otherwise.
  bool Set(Real value, Real slope_per_beat) noexcept;

  /// Updates value by duration.
  ///
  /// @param duration Duration in beats.
  /// @return True if value changed, false otherwise.
  bool Update(Real duration) noexcept;

 private:
  // Clamps given `value`.
  Real Clamp(Real value) noexcept;

  // Definition.
  ControlDefinition definition_;

  // Value.
  Real value_ = 0.0;

  // Slope in value change per beat.
  Real slope_per_beat_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CONTROL_H_
