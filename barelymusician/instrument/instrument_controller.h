#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_CONTROLLER_H_

#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"

namespace barelyapi {

class InstrumentController {
 public:
  explicit InstrumentController(
      const InstrumentParamDefinitions& param_definitions);

  std::vector<float> GetAllNotes() const;
  std::vector<Param> GetAllParams() const;

  const float* GetParam(int id) const;
  bool IsNoteOn(float pitch) const;

  void ResetAllParams();
  bool ResetParam(int id);
  void SetAllNotesOff();
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

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_CONTROLLER_H_
