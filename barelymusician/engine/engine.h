#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <map>
#include <unordered_map>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

// Class that wraps the internal BarelyMusician engine.
class Engine {
 public:
  /// Instrument note off callback signature.
  using InstrumentNoteOffCallback = InstrumentController::NoteOffCallback;

  /// Instrument note on callback signature.
  using InstrumentNoteOnCallback = InstrumentController::NoteOnCallback;

  // TODO(#85): Temp definition to allow callback setter.
  using BeatCallback = void (*)(double position, double timestamp,
                                void* user_data);
  using UpdateCallback = void (*)(double begin_position, double end_position,
                                  double begin_timestamp, double end_timestamp,
                                  void* user_data);

  /// Constructs new `Engine`.
  ///
  /// @param sample_rate System sampling rate in hz.
  explicit Engine(int sample_rate) noexcept;

  /// Adds new instrument.
  ///
  /// @param definition Instrument definition.
  /// @return Instrument id.
  Id AddInstrument(InstrumentDefinition definition) noexcept;

  /// Adds new performer.
  ///
  /// @return Performer id.
  Id AddPerformer() noexcept;

  /// Adds performer note.
  ///
  /// @param performer_id Performer id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Note id.
  StatusOr<Id> AddPerformerNote(Id performer_id, double position,
                                Note note) noexcept;

  /// Returns performer begin offset.
  ///
  /// @param performer_id Performer id.
  /// @return Begin offset in beats.
  StatusOr<double> GetPerformerBeginOffset(Id performer_id) const noexcept;

  /// Returns performer begin position.
  ///
  /// @param performer_id Performer id.
  /// @return Optional begin position in beats.
  StatusOr<double> GetPerformerBeginPosition(Id performer_id) const noexcept;

  /// Returns performer end position.
  ///
  /// @param performer_id Performer id.
  /// @return Optional end position in beats.
  StatusOr<double> GetPerformerEndPosition(Id performer_id) const noexcept;

  /// Returns performer loop begin offset.
  ///
  /// @param performer_id Performer id.
  /// @return Loop begin offset in beats.
  StatusOr<double> GetPerformerLoopBeginOffset(Id performer_id) const noexcept;

  /// Returns performer loop length.
  ///
  /// @param performer_id Performer id.
  /// @return Loop length in beats.
  StatusOr<double> GetPerformerLoopLength(Id performer_id) const noexcept;

  /// Returns the playback position.
  ///
  /// @return Position in beats.
  double GetPlaybackPosition() const noexcept;

  /// Returns the playback tempo.
  ///
  /// @return Tempo in bpm.
  double GetPlaybackTempo() const noexcept;

  /// Returns whether the performer is empty or not.
  ///
  /// @param performer_id Performer id.
  /// @return True if empty (i.e., has no notes), false if not, or error status.
  StatusOr<bool> IsPerformerEmpty(Id performer_id) const noexcept;

  /// Returns whether the performer is looping or not.
  ///
  /// @param performer_id Performer id.
  /// @return True if looping, false if not, or error status.
  StatusOr<bool> IsPerformerLooping(Id performer_id) const noexcept;

  /// Returns whether the playback is currently active or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void ProcessInstrument(Id instrument_id, double timestamp, float* output,
                         int num_channels, int num_frames) noexcept;

  /// Removes all performer notes.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemoveAllPerformerNotes(Id performer_id) noexcept;

  /// Removes all performer notes at range.
  ///
  /// @param performer_id Performer id.
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                 double end_position) noexcept;

  /// Removes instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status RemoveInstrument(Id instrument_id) noexcept;

  /// Removes performer.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemovePerformer(Id performer_id) noexcept;

  /// Removes performer note.
  ///
  /// @param performer_id Performer id.
  /// @param note_id Note id.
  /// @return Status.
  Status RemovePerformerNote(Id performer_id, Id note_id) noexcept;

  /// Sets all instrument notes off.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentNotesOff(Id instrument_id) noexcept;

  /// Sets all instrument parameters to default.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentParamsToDefault(Id instrument_id) noexcept;

  /// Sets conductor.
  ///
  /// @param definition Conductor definition.
  void SetConductor(ConductorDefinition definition) noexcept;

  /// Sets instrument data.
  ///
  /// @param instrument_id Instrument id.
  /// @param data Data.
  /// @return Status.
  Status SetInstrumentData(Id instrument_id, void* data) noexcept;

  /// Sets instrument gain.
  ///
  /// @param instrument_id Instrument id.
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetInstrumentGain(Id instrument_id, float gain) noexcept;

  /// Sets whether instrument should be muted or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetInstrumentMuted(Id instrument_id, bool is_muted) noexcept;

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetInstrumentNoteOff(Id instrument_id, float note_pitch) noexcept;

  /// Sets the instrument note off callback.
  ///
  /// @param instrument_id Instrument id.
  /// @param instrument_note_off_callback Instrument note off callback.
  void SetInstrumentNoteOffCallback(
      Id instrument_id,
      InstrumentNoteOffCallback instrument_note_off_callback) noexcept;

  /// Sets instrument note on.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                             float note_intensity) noexcept;

  /// Sets the instrument note on callback.
  ///
  /// @param instrument_id Instrument id.
  /// @param instrument_note_on_callback Instrument note on callback.
  void SetInstrumentNoteOnCallback(
      Id instrument_id,
      InstrumentNoteOnCallback instrument_note_on_callback) noexcept;

  /// Sets instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_index Parameter index.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetInstrumentParam(Id instrument_id, int param_index,
                            float param_value) noexcept;

  /// Sets instrument parameter to default.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_index Parameter index.
  /// @return Status.
  Status SetInstrumentParamToDefault(Id instrument_id,
                                     int param_index) noexcept;

  /// Sets performer begin offset.
  ///
  /// @param performer_id Performer id.
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetPerformerBeginOffset(Id performer_id, double begin_offset) noexcept;

  /// Sets performer begin position.
  ///
  /// @param performer_id Performer id.
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetPerformerBeginPosition(Id performer_id,
                                   double begin_position) noexcept;

  /// Sets performer end position.
  ///
  /// @param performer_id Performer id.
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetPerformerEndPosition(Id performer_id, double end_position) noexcept;

  /// Sets performer instrument.
  ///
  /// @param performer_id Performer id.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetPerformerInstrument(Id performer_id, Id instrument_id) noexcept;

  /// Sets whether performer should be looping or not.
  ///
  /// @param performer_id Performer id.
  /// @param loop True if looping.
  /// @return Status.
  Status SetPerformerLoop(Id performer_id, bool loop) noexcept;

  /// Sets performer loop begin offset.
  ///
  /// @param performer_id Performer id.
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetPerformerLoopBeginOffset(Id performer_id,
                                     double loop_begin_offset) noexcept;

  /// Sets performer loop length.
  ///
  /// @param performer_id Performer id.
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetPerformerLoopLength(Id performer_id, double loop_length) noexcept;

  /// Sets the playback beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @param user_data User data.
  void SetPlaybackBeatCallback(BeatCallback beat_callback,
                               void* user_data) noexcept;

  /// Sets the playback position.
  ///
  /// @param position Position in beats.
  void SetPlaybackPosition(double position) noexcept;

  /// Sets the playback tempo.
  ///
  /// @param tempo Tempo in bpm.
  void SetPlaybackTempo(double tempo) noexcept;

  /// Sets the playback update callback.
  ///
  /// @param update_callback Update callback.
  /// @param user_data User data.
  void SetPlaybackUpdateCallback(UpdateCallback update_callback,
                                 void* user_data) noexcept;

  /// Sets the sample rate.
  ///
  /// @param sample_rate Sampling rate in hz.
  void SetSampleRate(int sample_rate) noexcept;

  /// Starts the playback.
  void StartPlayback() noexcept;

  /// Stops the playback.
  void StopPlayback() noexcept;

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Active note that is being performed.
  struct ActiveNote {
    // Note end position.
    double end_position;

    // Note pitch.
    float pitch;
  };
  struct Performer {
    // List of active notes.
    std::multimap<double, ActiveNote> active_notes;

    // Instrument id to perform.
    Id instrument_id;

    // Sequence to perform.
    Sequence sequence;
  };

  // Updates sequences.
  void UpdateSequences(double begin_position, double end_position);

  // Conductor.
  Conductor conductor_;

  // Id generator.
  IdGenerator id_generator_;

  // Instrument manager.
  InstrumentManager instrument_manager_;

  // List of performers.
  std::unordered_map<Id, Performer> performers_;

  // Playback tempo in bpm.
  double playback_tempo_;

  // Playback transport.
  Transport transport_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
