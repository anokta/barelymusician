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
  // Note off callback signature.
  using NoteOffCallback = std::function<void(int64 timestamp, float pitch)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(int64 timestamp, float pitch, float intensity)>;

  explicit InstrumentController(InstrumentParamDefinitions param_definitions);

  std::vector<float> GetAllActiveNotes() const;
  std::vector<std::pair<int, float>> GetAllParams() const;

  const float* GetParam(int id) const;
  bool IsNoteOn(float pitch) const;

  void ResetAllParams();
  void SetAllNotesOff();

  bool SetData(int64 timestamp, InstrumentData data);

  void SetNoteOffCallback(NoteOffCallback note_off_callback);
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

 private:
  // Active note pitches.
  std::unordered_set<float> active_notes_;

  // Instrument params.
  std::unordered_map<int, std::pair<InstrumentParamDefinition, float>> params_;

  NoteOffCallback note_off_callback_;
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
