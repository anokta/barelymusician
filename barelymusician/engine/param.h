#ifndef BARELYMUSICIAN_ENGINE_PARAM_H_
#define BARELYMUSICIAN_ENGINE_PARAM_H_

#include <unordered_map>

#include "barelymusician/engine/param_definition.h"

namespace barely {

/// Class that wraps a parameter.
class Param {
 public:
  /// Constructs new |Param|.
  ///
  /// @param definition Parameter definition.
  explicit Param(ParamDefinition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Parameter definition.
  const ParamDefinition& GetDefinition() const noexcept;

  /// Returns value.
  ///
  /// @return Parameter value.
  float GetValue() const noexcept;

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

/// Parameter by id map type.
using ParamMap = std::unordered_map<int, Param>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAM_H_
