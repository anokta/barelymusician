#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <functional>
#include <map>
#include <unordered_set>
#include <vector>

#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

/// Class that wraps main thread functionality of an instrument.
class InstrumentController {
 public:
  /// Note off callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Note timestamp.
  using NoteOffCallback = std::function<void(float pitch, double timestamp)>;

  /// Note on callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Note timestamp.
  using NoteOnCallback =
      std::function<void(float pitch, float intensity, double timestamp)>;

  /// Constructs new `InstrumentController`.
  ///
  /// @param param_definitions List of parameter definitions.
  explicit InstrumentController(std::vector<ParamDefinition> param_definitions);

  /// Extracts events to be processed.
  ///
  /// @return Map of instrument events by their timestamps.
  std::multimap<double, InstrumentEvent> ExtractEvents();

  /// Returns all parameters.
  ///
  /// @return List of parameter values.
  std::vector<float> GetAllParams() const;

  /// Returns gain.
  ///
  /// @return gain Gain in amplitude.
  float GetGain() const;

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Pointer to parameter.
  const Param* GetParam(int index) const;

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise.
  bool IsMuted() const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  bool IsNoteOn(float pitch) const;

  // TODO(#85): Implement `ProcessEvent`?

  /// Resets all parameters to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ResetAllParams(double timestamp);

  /// Resets parameter to default value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetParam(int index, double timestamp);

  /// Sets data at timestamp.
  ///
  /// @param data Data.
  /// @param timestamp Timestamp in seconds.
  void SetData(void* data, double timestamp);

  /// Sets gain at timestamp.
  ///
  /// @param gain Gain in amplitude.
  /// @param timestamp Timestamp in seconds.
  void SetGain(float gain, double timestamp);

  /// Sets whether instrument should be muted or not at timestamp.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @param timestamp Timestamp in seconds.
  void SetMuted(bool is_muted, double timestamp);

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets parameter value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetParam(int index, float value, double timestamp);

  /// Starts note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Timestamp in seconds.
  void StartNote(float pitch, float intensity, double timestamp);

  /// Stops all notes at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void StopAllNotes(double timestamp);

  /// Stops note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  void StopNote(float pitch, double timestamp);

 private:
  // Map of events by their timestamps.
  std::multimap<double, InstrumentEvent> events_;

  // Gain in amplitude.
  float gain_;

  // Denotes whether instrument is muted or not.
  bool is_muted_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // List of parameters.
  std::vector<Param> params_;

  // List of active note pitches.
  std::unordered_set<float> pitches_;
};

}  // namespace barelyapi

#endif  //  BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
