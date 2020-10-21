#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace barelyapi {

// Instrument controller.
class InstrumentController {
 public:
  // Constructs new |InstrumentController|.
  //
  // @param params Default instrument params.
  explicit InstrumentController(
      const std::vector<std::pair<int, float>>& params);

  // Stops all notes.
  void AllNotesOff();

  // Returns all active notes.
  //
  // @return List of active note indices.
  std::vector<float> GetAllNotes() const;

  // Returns all params with their current values.
  //
  // @return List of param id-value pairs.
  std::vector<std::pair<int, float>> GetAllParams() const;

  // Returns parameter value with the given |id|.
  //
  // @param id Param id.
  // @return Param value.
  std::optional<float> GetParam(int id) const;

  // Returns whether note with the given |index| is on.
  //
  // @param index Note index.
  // @return True if note is on, false otherwise.
  bool IsNoteOn(float index) const;

  // Sets note with the given |index| off.
  //
  // @param index Note index.
  bool NoteOff(float index);

  // Sets note with the given |index| on.
  //
  // @param index Note index.
  bool NoteOn(float index);

  // Resets all params.
  void ResetAllParams();

  // Sets the given |value| to the parameter with the given |id|.
  //
  // @param id Param id.
  // @param value Param value.
  std::optional<bool> SetParam(int id, float value);

 private:
  // Instrument parameter.
  struct InstrumentParam {
    // Current parameter value.
    float value;

    // Default parameter value.
    float default_value;
  };

  // Instrument params.
  std::unordered_map<int, InstrumentParam> params_;

  // Active note indices.
  std::unordered_set<float> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
