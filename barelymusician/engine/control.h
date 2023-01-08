#ifndef BARELYMUSICIAN_ENGINE_CONTROL_H_
#define BARELYMUSICIAN_ENGINE_CONTROL_H_

#include "barelymusician/barelymusician.h"

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

  /// Returns value.
  ///
  /// @return Control value.
  [[nodiscard]] double Get() const noexcept;

  /// Resets value.
  ///
  /// @return True if changed, false otherwise.
  bool Reset() noexcept;

  /// Sets value.
  ///
  /// @param value Control value.
  /// @param slope_per_second Control slope in value change per second.
  /// @return True if changed, false otherwise.
  bool Set(double value, double slope_per_second) noexcept;

  /// Updates value by elapsed seconds.
  ///
  /// @param elapsed_seconds Elapsed seconds.
  /// @return True if value changed, false otherwise.
  bool UpdateBy(double elapsed_seconds) noexcept;

 private:
  // Clamps given `value`.
  double Clamp(double value) noexcept;

  // Definition.
  ControlDefinition definition_;

  // Value.
  double value_ = 0.0;

  // Slope in value change per second.
  double slope_per_second_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CONTROL_H_
