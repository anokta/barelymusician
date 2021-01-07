#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/base/types.h"
#include "barelymusician/engine/instrument_data.h"
#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

class InstrumentController {
 public:
  explicit InstrumentController(
      const InstrumentParamDefinitions& param_definitions);

  std::vector<float> GetAllNotes() const;
  std::vector<Param> GetAllParams() const;

  const float* GetParam(int id) const;
  bool IsNoteOn(float pitch) const;

  bool ResetParam(int id);
  bool SetNoteOff(float pitch);
  bool SetNoteOn(float pitch);
  bool SetParam(int id, float value);

 private:
  // Active note pitches.
  std::unordered_set<float> notes_;

  // Instrument params.
  std::unordered_map<int, std::pair<InstrumentParamDefinition, float>> params_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
