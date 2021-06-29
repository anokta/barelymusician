#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <any>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "barelymusician/common/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"
#include "barelymusician/engine/instrument_param_definition.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

/// Class that manages processing of instruments.
class InstrumentManager {
 public:
  /// Note off callback signature.
  using NoteOffCallback =
      std::function<void(Id instrument_id, double timestamp, float note_pitch)>;

  /// Note on callback signature.
  using NoteOnCallback =
      std::function<void(Id instrument_id, double timestamp, float note_pitch,
                         float note_intensity)>;

  /// Constructs new |InstrumentManager|.
  InstrumentManager();

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches.
  const std::unordered_set<float>* GetAllNotes(Id instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters.
  const std::unordered_map<int, InstrumentParam>* GetAllParams(
      Id instrument_id) const;

  /// Returns instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Pointer to parameter if successful, nullptr otherwise.
  const InstrumentParam* GetParam(Id instrument_id, int param_id) const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, false otherwise.
  bool IsNoteOn(Id instrument_id, float note_pitch) const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param sample_rate Sampling rate in Hz.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return True if successful, false otherwise.
  void Process(Id instrument_id, double timestamp, int sample_rate,
               float* output, int num_channels, int num_frames);

  /// Schedules all notes of all instruments off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ScheduleAllNotesOff(double timestamp);

  /// Schedules all instrument notes off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  void ScheduleAllNotesOff(Id instrument_id, double timestamp);

  /// Schedules all parameters of all instruments to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ScheduleAllParamsToDefault(double timestamp);

  /// Schedules all instrument parameters to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  void ScheduleAllParamsToDefault(Id instrument_id, double timestamp);

  /// Schedules create new instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  void ScheduleCreate(Id instrument_id, double timestamp,
                      InstrumentDefinition definition,
                      InstrumentParamDefinitions param_definitions);

  /// Schedules destroy instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  void ScheduleDestroy(Id instrument_id, double timestamp);

  /// Schedules custom instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param custom_data Custom data.
  void ScheduleCustomData(Id instrument_id, double timestamp,
                          std::any custom_data);

  /// Schedules instrument note off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  void ScheduleNoteOff(Id instrument_id, double timestamp, float note_pitch);

  /// Schedules instrument note on at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  void ScheduleNoteOn(Id instrument_id, double timestamp, float note_pitch,
                      float note_intensity);

  /// Schedules instrument parameter value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  void ScheduleParam(Id instrument_id, double timestamp, int param_id,
                     float param_value);

  /// Schedules instrument parameter to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_id Parameter id.
  void ScheduleParamToDefault(Id instrument_id, double timestamp, int param_id);

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets the note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Updates state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Instrument controller that wraps the main thread calls of an instrument.
  struct InstrumentController {
    // List of instrument parameters.
    std::unordered_map<int, InstrumentParam> params;

    // List of active note pitches.
    std::unordered_set<float> pitches;
  };

  // Instrument processor that wraps the audio thread calls of an instrument.
  struct InstrumentProcessor {
    // List of scheduled instrument events.
    InstrumentEvents events;

    // Instrument.
    std::optional<Instrument> instrument;
  };

  // TODO: this should be done once (instead of called from each scheduler)?
  void ScheduleProcessorEvents(Id instrument_id, InstrumentEvents events);

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Audio thread task runner.
  TaskRunner task_runner_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;

  // TODO: implement Scheduler<EventType>.
  using Event = std::function<void()>;
  using Events = std::multimap<double, Event>;

  Events main_events_;
  std::vector<Event> audio_events_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
