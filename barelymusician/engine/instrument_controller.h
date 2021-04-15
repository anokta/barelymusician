#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

// Instrument controller not off callback signature.
using InstrumentNoteOffCallback = std::function<void(float pitch)>;

// Instrument controller not on callback signature.
using InstrumentNoteOnCallback =
    std::function<void(float pitch, float intensity)>;

/// Instrument parameter.
struct InstrumentParam {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const InstrumentParam&) const = default;
};

/// Instrument controller that wraps the main thread calls of an instrument.
class InstrumentController {
 public:
  /// Constructs new |InstrumentController|.
  ///
  /// @param definitions Instrument parameter definitions.
  InstrumentController(const InstrumentParamDefinitions& definitions,
                       InstrumentNoteOffCallback note_off_callback,
                       InstrumentNoteOnCallback note_on_callback);

  /// Returns all active notes.
  ///
  /// @return List of active note pitches.
  std::vector<float> GetAllNotes() const;

  /// Returns all parameters.
  ///
  /// @return List of all instrument parameters.
  std::vector<InstrumentParam> GetAllParams() const;

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
  // List of scheduled events.
  InstrumentControllerEvents events_;

  // Instrument note off callback.
  InstrumentNoteOffCallback note_off_callback_;

  // Instrument note on callback.
  InstrumentNoteOnCallback note_on_callback_;

  // List of instrument parameters.
  std::unordered_map<int, std::pair<InstrumentParamDefinition, float>> params_;

  // List of active note pitches.
  std::unordered_set<float> pitches_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
