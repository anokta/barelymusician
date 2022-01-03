#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <any>
#include <atomic>
#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor_event.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

/// Class that manages processing of instruments.
class InstrumentManager {
 public:
  /// Note off callback signature.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  using NoteOffCallback =
      std::function<void(Id instrument_id, float note_pitch)>;

  /// Note on callback signature.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  using NoteOnCallback = std::function<void(Id instrument_id, float note_pitch,
                                            float note_intensity)>;

  /// Constructs new |InstrumentManager|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit InstrumentManager(int sample_rate) noexcept;

  /// Adds new instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param definition Instrument definition.
  /// @return Status.
  Status Add(Id instrument_id, double timestamp,
             InstrumentDefinition definition) noexcept;

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

  /// Returns whether instrument is valid or not.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if instrument is valid (i.e., exists), false otherwise.
  bool IsValid(Id instrument_id) const noexcept;

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

  /// Removes instrument at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Remove(Id instrument_id, double timestamp) noexcept;

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

  /// Sets custom instrument data at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param custom_data Custom data.
  /// @return Status.
  Status SetCustomData(Id instrument_id, double timestamp,
                       std::any custom_data) noexcept;

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
  void SetNoteOffCallback(NoteOffCallback note_off_callback) noexcept;

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
  void SetNoteOnCallback(NoteOnCallback note_on_callback) noexcept;

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

  /// Sets sampling rate at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(double timestamp, int sample_rate) noexcept;

  /// Updates the internal state.
  void Update() noexcept;

 private:
  // Instrument controller that wraps the main thread calls of an instrument.
  struct InstrumentController {
    // Constructs new |InstrumentController|.
    explicit InstrumentController(InstrumentDefinition definition) noexcept;

    // Instrument definition.
    InstrumentDefinition definition;

    // Instrument gain.
    float gain;

    // Denotes whether instrument is muted or not.
    bool is_muted;

    // Instrument parameters.
    std::vector<Param> params;

    // List of active note pitches.
    std::unordered_set<float> pitches;
  };

  // Instrument processor that wraps the audio thread calls of an instrument.
  struct InstrumentProcessor {
    // Instrument.
    std::optional<Instrument> instrument;

    // List of scheduled instrument events.
    InstrumentProcessorEventMap events;
  };

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;
  std::unordered_map<Id, InstrumentProcessorEventMap> update_events_;

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
