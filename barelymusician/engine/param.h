#ifndef BARELYMUSICIAN_ENGINE_PARAM_H_
#define BARELYMUSICIAN_ENGINE_PARAM_H_

#include "barelymusician/engine/param_definition.h"

namespace barely {

/// Class that wraps a parameter.
class Param {
 public:
  /// Constructs new |Param|.
  ///
  /// @param definition Parameter definition.
  explicit Param(ParamDefinition definition);

  /// Returns definition.
  ///
  /// @return Parameter definition.
  const ParamDefinition& GetDefinition() const;

  /// Returns id.
  ///
  /// @return Parameter id.
  int GetId() const;

  /// Returns value.
  ///
  /// @return Parameter value.
  float GetValue() const;

  /// Resets value.
  ///
  /// @return True if successful, false otherwise.
  bool ResetValue();

  /// Sets value.
  ///
  /// @param value Parameter value.
  /// @return True if successful, false otherwise.
  bool SetValue(float value);

 private:
  // Parameter definition.
  ParamDefinition definition_;

  // Parameter value.
  float value_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAM_H_
