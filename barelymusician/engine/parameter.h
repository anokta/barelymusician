#ifndef BARELYMUSICIAN_ENGINE_PARAMETER_H_
#define BARELYMUSICIAN_ENGINE_PARAMETER_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps parameter.
class Parameter {
 public:
  /// Definition alias.
  using Definition = barely::ParameterDefinition;

  /// Constructs new `Parameter`.
  ///
  /// @param definition Parameter definition.
  explicit Parameter(Definition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Parameter definition.
  [[nodiscard]] const Definition& GetDefinition() const noexcept;

  /// Returns value.
  ///
  /// @return Parameter value.
  [[nodiscard]] double GetValue() const noexcept;

  /// Resets value.
  ///
  /// @return True if successful, false otherwise.
  bool ResetValue() noexcept;

  /// Sets value.
  ///
  /// @param value Parameter value.
  /// @return True if successful, false otherwise.
  bool SetValue(double value) noexcept;

 private:
  // Definition.
  Definition definition_;

  // Value.
  double value_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_PARAMETER_H_
