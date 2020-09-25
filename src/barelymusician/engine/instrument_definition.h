#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

// Instrument parameter definition.
struct InstrumentParamDefinition {
  // Param id.
  int id;

  // Param name.
  std::string name;

  // Param description.
  std::string description;

  // Param default value.
  float default_value;

  // Param minimum value.
  float min_value;

  // Param maximum value.
  float max_value;
};

// Instrument definition.
struct InstrumentDefinition {
  // TODO(#34): Sampling rate should probably be passed after this call instead.
  using GetInstrument =
      std::function<std::unique_ptr<Instrument>(int sample_rate)>;

  // Instrument name.
  std::string name;

  // Instrument description.
  std::string description;

  // Instrument parameters.
  std::vector<InstrumentParamDefinition> param_definitions;

  // Get instrument function.
  GetInstrument get_instrument_fn;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
