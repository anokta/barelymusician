#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_

#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

/// Class that wraps an instrument parameter.
class InstrumentParam {
 public:
  /// Constructs new |InstrumentParam|.
  explicit InstrumentParam(InstrumentParamDefinition definition);

  /// Returns value.
  ///
  /// @return Parameter value.
  float GetValue() const;

  /// Resets value.
  void ResetValue();

  /// Sets value.
  ///
  /// @param value Parameter value.
  void SetValue(float value);

 private:
  /// Parameter definition.
  InstrumentParamDefinition definition_;

  /// Parameter value.
  float value_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
