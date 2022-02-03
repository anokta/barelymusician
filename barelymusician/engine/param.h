#ifndef BARELYMUSICIAN_ENGINE_PARAM_H_
#define BARELYMUSICIAN_ENGINE_PARAM_H_

#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

/// Class that wraps a parameter.
class Param {
 public:
  /// Constructs new `Param`.
  ///
  /// @param definition Parameter definition.
  explicit Param(ParamDefinition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Parameter definition.
  [[nodiscard]] const ParamDefinition& GetDefinition() const noexcept;

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
  ParamDefinition definition_;

  // Parameter value.
  float value_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PARAM_H_
