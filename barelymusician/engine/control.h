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
  [[nodiscard]] double GetValue() const noexcept;

  /// Resets value.
  ///
  /// @return True if successful, false otherwise.
  bool ResetValue() noexcept;

  /// Sets value.
  ///
  /// @param value Control value.
  /// @return True if successful, false otherwise.
  bool SetValue(double value) noexcept;

 private:
  // Definition.
  ControlDefinition definition_;

  // Value.
  double value_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CONTROL_H_
