#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

/// Class that manages processing of instruments.
class InstrumentManager {
 public:
  /// Constructs new `InstrumentManager`.
  InstrumentManager() noexcept;

  /// Adds new instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in hz.
  /// @return Status.
  Status Create(Id instrument_id, InstrumentDefinition definition,
                int sample_rate) noexcept;

  /// Removes instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status Destroy(Id instrument_id) noexcept;

  /// Returns instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_index Parameter index.
  /// @return Instrument parameter or error status.
  StatusOr<Param> GetParam(Id instrument_id, int param_index) const noexcept;

  /// Returns instrument gain.
  ///
  /// @param instrument_id Instrument id.
  /// @return Instrument gain or error status.
  StatusOr<float> GetGain(Id instrument_id) const noexcept;

  /// Returns whether instrument is muted or not.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if muted, or false if not, or error status.
  StatusOr<bool> IsMuted(Id instrument_id) const noexcept;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, or false if not, or error status.
  StatusOr<bool> IsNoteOn(Id instrument_id, float note_pitch) const noexcept;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(Id instrument_id, double timestamp, float* output,
               int num_channels, int num_frames) noexcept;

  /// Processes instrument event at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param event Instrument event.
  void ProcessEvent(Id instrument_id, double timestamp,
                    InstrumentEvent event) noexcept;

  /// Sets all notes of all instruments off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllNotesOff(double timestamp) noexcept;

  /// Sets all instrument notes off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetAllNotesOff(Id instrument_id, double timestamp) noexcept;

  /// Sets all parameters of all instruments to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetAllParamsToDefault(double timestamp) noexcept;

  /// Sets all instrument parameters to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetAllParamsToDefault(Id instrument_id, double timestamp) noexcept;

  /// Sets instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param data Data.
  /// @return Status.
  Status SetData(Id instrument_id, double timestamp, void* data) noexcept;

  /// Sets instrument gain at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetGain(Id instrument_id, double timestamp, float gain) noexcept;

  /// Sets whether instrument should be muted or not at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetMuted(Id instrument_id, double timestamp, bool is_muted) noexcept;

  /// Sets instrument note off at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(Id instrument_id, double timestamp,
                    float note_pitch) noexcept;

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  /// @return Status.
  Status SetNoteOffCallback(
      Id instrument_id,
      InstrumentController::NoteOffCallback note_off_callback) noexcept;

  /// Sets instrument note on at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status SetNoteOn(Id instrument_id, double timestamp, float note_pitch,
                   float note_intensity) noexcept;

  /// Sets the note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  /// @return Status.
  Status SetNoteOnCallback(
      Id instrument_id,
      InstrumentController::NoteOnCallback note_on_callback) noexcept;

  /// Sets instrument parameter value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_index Parameter index.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetParam(Id instrument_id, double timestamp, int param_index,
                  float param_value) noexcept;

  /// Sets instrument parameter to default value at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param param_index Parameter index.
  /// @return Status.
  Status SetParamToDefault(Id instrument_id, double timestamp,
                           int param_index) noexcept;

  /// Updates the internal state.
  void Update() noexcept;

 private:
  // Instrument controller by id map.
  std::unordered_map<Id, InstrumentController> controllers_;

  // Instrument controller by id map.
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Instrument update events by id map.
  std::unordered_map<Id, std::multimap<double, InstrumentEvent>> update_events_;

  // Audio thread task runner.
  TaskRunner runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
