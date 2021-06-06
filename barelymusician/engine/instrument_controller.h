#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"

namespace barelyapi {

/// Instrument controller that wraps the main thread calls of an instrument.
class InstrumentController {
 public:
  // Instrument controller not off callback signature.
  using NoteOffCallback = std::function<void(float pitch)>;

  // Instrument controller not on callback signature.
  using NoteOnCallback = std::function<void(float pitch, float intensity)>;

  /// Constructs new |InstrumentController|.
  ///
  /// @param definitions Instrument parameter definitions.
  InstrumentController(InstrumentDefinition definition,
                       const InstrumentParamDefinitions& param_definitions,
                       NoteOffCallback note_off_callback,
                       NoteOnCallback note_on_callback);

  /// Destroys |InstrumentController|.
  ~InstrumentController();

  /// Copyable and movable.
  InstrumentController(const InstrumentController& other) = default;
  InstrumentController& operator=(const InstrumentController& other) = default;
  InstrumentController(InstrumentController&& other) = default;
  InstrumentController& operator=(InstrumentController&& other) = default;

  /// Returns all active notes.
  ///
  /// @return List of active note pitches.
  std::vector<float> GetAllNotes() const;

  /// Returns all parameters.
  ///
  /// @return List of all instrument parameters.
  std::vector<InstrumentParam> GetAllParams() const;

  InstrumentDefinition GetDefinition() const;

  /// Returns parameter value.
  ///
  /// @param id Parameter id.
  /// @return Pointer to the parameter value, or nullptr if not found.
  const float* GetParam(int id) const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  bool IsNoteOn(float pitch) const;

  void Schedule(double timestamp, InstrumentControllerEvent event);
  void Schedule(InstrumentControllerEvents events);

  InstrumentProcessorEvents Update(double timestamp);

 private:
  // Instrument definition.
  InstrumentDefinition definition_;

  // List of scheduled events.
  InstrumentControllerEvents events_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;

  // List of instrument parameters.
  std::unordered_map<int, std::pair<InstrumentParamDefinition, float>> params_;

  // List of active note pitches.
  std::unordered_set<float> pitches_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
