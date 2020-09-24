#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <string>
#include <vector>

namespace barelyapi {

// Instrument parameter definition.
struct InstrumentParamDefinition {
  // Param id.
  int id;

  // Param name.
  std::string name;

  // Param description.
  std::string description;

  // Param value.
  float default_value;
};

// Instrument definition.
struct InstrumentDefinition {
  // Instrument name.
  std::string name;

  // Instrument description.
  std::string description;

  // Instrument parameters.
  std::vector<InstrumentParamDefinition> params;
};

//// Returns definition of the given instrument type.
//// @note All derived instrument types *must* implement this function.
////
//// @return Instrument definition.
//template <typename InstrumentType>
//static InstrumentDefinition GetDefinition();

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
