#ifndef BARELYMUSICIAN_INSTRUMENT_PARAMETER_H_
#define BARELYMUSICIAN_INSTRUMENT_PARAMETER_H_

#include "barelymusician/api/instrument.h"

namespace barelyapi {

/// Class that wraps a parameter.
class Parameter {
 public:
  /// Constructs new `Parameter`.
  ///
  /// @param definition Parameter definition.
  explicit Parameter(barely::ParameterDefinition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Parameter definition.
  [[nodiscard]] const barely::ParameterDefinition& GetDefinition()
      const noexcept;

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
  // Parameter definition.
  barely::ParameterDefinition definition_;

  // Parameter value.
  double value_ = 0.0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_PARAMETER_H_
