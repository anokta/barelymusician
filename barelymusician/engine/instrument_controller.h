#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <functional>
#include <map>
#include <unordered_set>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/parameter.h"

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
  /// @param definition Instrument definition.
  explicit InstrumentController(const InstrumentDefinition& definition);

  /// Returns events to be processed.
  ///
  /// @return Map of instrument events by their timestamps.
  std::multimap<double, InstrumentEvent>& GetEvents();

  /// Returns gain.
  ///
  /// @return gain Gain in amplitude.
  [[nodiscard]] float GetGain() const;

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Pointer to parameter.
  [[nodiscard]] const Parameter* GetParameter(int index) const;

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise.
  [[nodiscard]] bool IsMuted() const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const;

  /// Processes event at timestamp.
  ///
  /// @param event Instrument event.
  /// @param timestamp Timestamp in seconds.
  void ProcessEvent(const InstrumentEvent& event, double timestamp);

  /// Resets all parameters to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ResetAllParameters(double timestamp);

  /// Resets parameter to default value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetParameter(int index, double timestamp);

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
  bool SetParameter(int index, float value, double timestamp);

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
  float gain_ = 1.0f;

  // Denotes whether instrument is muted or not.
  bool is_muted_ = false;

  // Note off callback.
  NoteOffCallback note_off_callback_ = nullptr;

  // Note on callback.
  NoteOnCallback note_on_callback_ = nullptr;

  // List of parameters.
  std::vector<Parameter> parameters_;

  // List of active note pitches.
  std::unordered_set<float> pitches_;
};

}  // namespace barelyapi

#endif  //  BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
