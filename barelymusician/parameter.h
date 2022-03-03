#ifndef BARELYMUSICIAN_PARAMETER_H_
#define BARELYMUSICIAN_PARAMETER_H_

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Parameter definition alias.
using ParameterDefinition = BarelyParameterDefinition;

/// Class that wraps a parameter.
class Parameter {
 public:
  /// Constructs new `Parameter`.
  ///
  /// @param definition Parameter definition.
  explicit Parameter(ParameterDefinition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Parameter definition.
  [[nodiscard]] const ParameterDefinition& GetDefinition() const noexcept;

  /// Returns value.
  ///
  /// @return Parameter value.
  [[nodiscard]] float GetValue() const noexcept;

  /// Resets value.
  ///
  /// @return True if successful, false otherwise.
  bool ResetValue() noexcept;

  /// Sets value.
  ///
  /// @param value Parameter value.
  /// @return True if successful, false otherwise.
  bool SetValue(float value) noexcept;

 private:
  // Parameter definition.
  ParameterDefinition definition_;

  // Parameter value.
  float value_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PARAMETER_H_