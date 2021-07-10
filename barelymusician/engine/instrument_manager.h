#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <any>
#include <atomic>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
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
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit InstrumentManager(int sample_rate);

  /// Creates new instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Status.
  Status Create(Id instrument_id, double timestamp,
                InstrumentDefinition definition,
                InstrumentParamDefinitions param_definitions);

  /// Destroys instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Destroy(Id instrument_id, double timestamp);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches or error status.
  StatusOr<std::vector<float>> GetAllNotes(Id instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters or error status.
  StatusOr<std::vector<InstrumentParam>> GetAllParams(Id instrument_id) const;

  /// Returns instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Instrument parameter or error status.
  StatusOr<InstrumentParam> GetParam(Id instrument_id, int param_id) const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, or false if not, or error status.
  StatusOr<bool> IsNoteOn(Id instrument_id, float note_pitch) const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(Id instrument_id, double timestamp, float* output,
               int num_channels, int num_frames);

  /// Sets all notes of all instruments off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllNotesOff(double timestamp);

  /// Sets all instrument notes off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetAllNotesOff(Id instrument_id, double timestamp);

  /// Sets all parameters of all instruments to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllParamsToDefault(double timestamp);

  /// Sets all instrument parameters to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetAllParamsToDefault(Id instrument_id, double timestamp);

  /// Sets custom instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param custom_data Custom data.
  /// @return Status.
  Status SetCustomData(Id instrument_id, double timestamp,
                       std::any custom_data);

  /// Sets instrument note off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(Id instrument_id, double timestamp, float note_pitch);

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets instrument note on at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status SetNoteOn(Id instrument_id, double timestamp, float note_pitch,
                   float note_intensity);

  /// Sets the note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets instrument parameter value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetParam(Id instrument_id, double timestamp, int param_id,
                  float param_value);

  /// Sets instrument parameter to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_id Parameter id.
  /// @return Status.
  Status SetParamToDefault(Id instrument_id, double timestamp, int param_id);

  /// Sets sampling rate at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(double timestamp, int sample_rate);

  /// Updates the internal state.
  void Update();

 private:
  // Instrument controller that wraps the main thread calls of an instrument.
  struct InstrumentController {
    // Constructs new |InstrumentController|.
    InstrumentController(InstrumentDefinition definition,
                         InstrumentParamDefinitions param_definitions);

    // Instrument definition.
    InstrumentDefinition definition;

    // List of instrument parameters.
    std::unordered_map<int, InstrumentParam> params;

    // List of active note pitches.
    std::unordered_set<float> pitches;
  };

  // Instrument processor that wraps the audio thread calls of an instrument.
  struct InstrumentProcessor {
    // Instrument.
    std::optional<Instrument> instrument;

    // List of scheduled instrument events.
    InstrumentEvents events;
  };

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;
  std::unordered_map<Id, InstrumentEvents> update_events_;

  // Audio thread task runner.
  TaskRunner audio_runner_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;

  // Sampling rate in Hz.
  std::atomic<int> sample_rate_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
