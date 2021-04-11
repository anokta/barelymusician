#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <variant>
#include <vector>

#include "barelymusician/common/id_generator.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

// TODO: refactor.
/// Instrument event type.
using InstrumentEvent =
    std::variant<ResetAllParams, ResetParam, SetAllNotesOff, SetCustomData,
                 SetNoteOff, SetNoteOn, SetParam>;

/// Class that manages processing of instruments.
class InstrumentManager {
 public:
  /// Note off callback signature.
  using NoteOffCallback =
      std::function<void(Id instrument_id, float note_pitch)>;

  /// Note on callback signature.
  using NoteOnCallback = std::function<void(Id instrument_id, float note_pitch,
                                            float note_intensity)>;

  /// Constructs new |InstrumentManager|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param id_generator Pointer to id generator.
  InstrumentManager(int sample_rate, IdGenerator* id_generator);

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  Id Create(InstrumentDefinition definition,
            InstrumentParamDefinitions param_definitions);

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  bool Destroy(Id instrument_id);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches.
  std::vector<float> GetAllNotes(Id instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters.
  std::vector<InstrumentParam> GetAllParams(Id instrument_id) const;

  /// Returns instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Pointer to parameter value if successful, nullptr otherwise.
  const float* GetParam(Id instrument_id, int param_id) const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, false otherwise.
  bool IsNoteOn(Id instrument_id, float note_pitch) const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool Process(Id instrument_id, float* output, int num_channels,
               int num_frames, double timestamp = 0.0);

  /// Resets all parameters of all instruments to their default values at
  /// timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ResetAllParams(double timestamp = 0.0);

  /// Resets all instrument parameters to their default values at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetAllParams(Id instrument_id, double timestamp = 0.0);

  /// Resets instrument parameter to its default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetParam(Id instrument_id, int param_id, double timestamp = 0.0);

  /// Sets all active notes of all instruments off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllNotesOff(double timestamp = 0.0);

  /// Sets all active instrument notes off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetAllNotesOff(Id instrument_id, double timestamp = 0.0);

  /// Sets custom instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param custom_data Custom data.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetCustomData(Id instrument_id, std::any custom_data,
                     double timestamp = 0.0);

  /// Sets instrument events at their timestamps.
  ///
  /// @param instrument_id Instrument id.
  /// @param events List of events with their timestamps.
  /// @return True if successful, false otherwise.
  bool SetEvents(Id instrument_id,
                 std::multimap<double, InstrumentEvent> events);

  /// Sets instrument note off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetNoteOff(Id instrument_id, float note_pitch, double timestamp = 0.0);

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets instrument note on at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetNoteOn(Id instrument_id, float note_pitch, float note_intensity,
                 double timestamp = 0.0);

  /// Sets the note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets instrument parameter value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetParam(Id instrument_id, int param_id, float param_value,
                double timestamp = 0.0);

  /// Sets the sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(int sample_rate);

  void Update(double timestamp = 0.0);

 private:
  // Sampling rate in Hz.
  int sample_rate_;

  // Instrument id generator.
  IdGenerator* id_generator_;  // not owned.

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Audio thread task runner.
  TaskRunner task_runner_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
