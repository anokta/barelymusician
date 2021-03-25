#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

// TODO: refactor.
/// Parameter event.
struct Param {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const Param&) const = default;
};

/// Instrument controller that wraps the main thread calls of an instrument.
class InstrumentController {
 public:
  /// Constructs new |InstrumentController|.
  ///
  /// @param definitions Instrument parameter definitions.
  explicit InstrumentController(const InstrumentParamDefinitions& definitions);

  /// Returns all active notes.
  ///
  /// @return List of active note pitches.
  std::vector<float> GetAllNotes() const;

  /// Returns all parameters.
  ///
  /// @return List of all instrument parameters.
  std::vector<Param> GetAllParams() const;

  /// Returns parameter value.
  ///
  /// @param id Parameter id.
  /// @return Pointer to the parameter value, or nullptr if not found.
  const float* GetParam(int id) const;

  const float* GetDefaultParam(int id) const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  bool IsNoteOn(float pitch) const;

  /// Resets all parameters.
  void ResetAllParams();

  /// Resets parameter.
  ///
  /// @param id Parameter id.
  /// @return True if successful.
  bool ResetParam(int id);

  /// Sets all active notes off.
  void SetAllNotesOff();

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  /// @return True if successful.
  bool SetNoteOff(float pitch);

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  /// @return True if successful.
  bool SetNoteOn(float pitch);

  /// Sets parameter.
  ///
  /// @param id Parameter id.
  /// @param value Parameter value.
  /// @return True if successful.
  bool SetParam(int id, float value);

 private:
  // List of active note pitches.
  std::unordered_set<float> pitches_;

  // List of instrument parameters.
  std::unordered_map<int, std::pair<InstrumentParamDefinition, float>> params_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
