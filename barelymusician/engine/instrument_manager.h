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
  InstrumentManager();

  /// Creates new instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  Status Create(Id instrument_id, double timestamp,
                InstrumentDefinition definition,
                InstrumentParamDefinitions param_definitions);

  /// Destroys instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  Status Destroy(Id instrument_id, double timestamp);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches.
  StatusOr<std::vector<float>> GetAllNotes(Id instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters.
  StatusOr<std::vector<InstrumentParam>> GetAllParams(Id instrument_id) const;

  /// Returns instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Pointer to parameter if successful, nullptr otherwise.
  StatusOr<InstrumentParam> GetParam(Id instrument_id, int param_id) const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, false otherwise.
  StatusOr<bool> IsNoteOn(Id instrument_id, float note_pitch) const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param sample_rate Sampling rate in Hz.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return True if successful, false otherwise.
  Status Process(Id instrument_id, double timestamp, int sample_rate,
                 float* output, int num_channels, int num_frames);

  /// Sets all notes of all instruments off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllNotesOff(double timestamp);

  /// Sets all instrument notes off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  Status SetAllNotesOff(Id instrument_id, double timestamp);

  /// Sets all parameters of all instruments to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllParamsToDefault(double timestamp);

  /// Sets all instrument parameters to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  Status SetAllParamsToDefault(Id instrument_id, double timestamp);

  /// Sets custom instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param custom_data Custom data.
  Status SetCustomData(Id instrument_id, double timestamp,
                       std::any custom_data);

  /// Sets instrument note off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
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
  Status SetParam(Id instrument_id, double timestamp, int param_id,
                  float param_value);

  /// Sets instrument parameter to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_id Parameter id.
  Status SetParamToDefault(Id instrument_id, double timestamp, int param_id);

  /// Updates the internal state.
  void Update();

 private:
  struct InstrumentData {
    explicit InstrumentData(InstrumentParamDefinitions param_definitions) {
      for (auto& param_definition : param_definitions) {
        params.emplace(param_definition.id,
                       InstrumentParam(std::move(param_definition)));
      }
    }

    // List of instrument parameters.
    std::unordered_map<int, InstrumentParam> params;

    // List of active note pitches.
    std::unordered_set<float> pitches;
  };

  // Instrument controller that wraps the main thread calls of an instrument.
  struct InstrumentController {
    InstrumentController(double timestamp, InstrumentDefinition definition,
                         InstrumentParamDefinitions param_definitions)
        : data(std::move(param_definitions)) {
      events.emplace(timestamp, CreateEvent{std::move(definition)});
      for (const auto& [id, param] : data->params) {
        events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
      }
    }

    std::optional<InstrumentData> data;

    // List of scheduled instrument events.
    InstrumentEvents events;
  };

  // Instrument processor that wraps the audio thread calls of an instrument.
  struct InstrumentProcessor {
    // List of scheduled instrument events.
    InstrumentEvents events;

    // Instrument.
    std::optional<Instrument> instrument;
  };

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
