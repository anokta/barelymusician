#ifndef BARELYMUSICIAN_ENGINE_CONTROL_H_
#define BARELYMUSICIAN_ENGINE_CONTROL_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps control.
class Control {
 public:
  /// Definition alias.
  using Definition = barely::ControlDefinition;

  /// Constructs new `Control`.
  ///
  /// @param definition Control definition.
  explicit Control(Definition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Control definition.
  [[nodiscard]] const Definition& GetDefinition() const noexcept;

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
  Definition definition_;

  // Value.
  double value_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CONTROL_H_
