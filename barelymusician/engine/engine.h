#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <map>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/task_runner.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

// Class that wraps the internal BarelyMusician engine.
class Engine {
 public:
  /// Beat callback signature.
  using BeatCallback = Transport::BeatCallback;

  /// Constructs new `Engine`.
  Engine() noexcept;

  /// Adds new sequence.
  ///
  /// @return Sequence identifier.
  Id AddSequence() noexcept;

  /// Adds sequence note.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param position Note position.
  /// @param note Note.
  /// @return Note id.
  StatusOr<Id> AddSequenceNote(Id sequence_id, double position,
                               BarelyNoteDefinition note) noexcept;

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in hz.
  /// @return Instrument id.
  Id CreateInstrument(BarelyInstrumentDefinition definition,
                      int sample_rate) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status DestroyInstrument(Id instrument_id) noexcept;

  /// Returns instrument gain.
  ///
  /// @param instrument_id Instrument id.
  /// @return Instrument gain or error status.
  [[nodiscard]] StatusOr<float> GetInstrumentGain(
      Id instrument_id) const noexcept;

  /// Returns instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param index Parameter index.
  /// @return Instrument parameter or error status.
  [[nodiscard]] StatusOr<Parameter> GetInstrumentParameter(
      Id instrument_id, int index) const noexcept;

  /// Returns sequence begin offset.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Begin offset in beats.
  [[nodiscard]] StatusOr<double> GetSequenceBeginOffset(
      Id sequence_id) const noexcept;

  /// Returns sequence begin position.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Optional begin position in beats.
  [[nodiscard]] StatusOr<double> GetSequenceBeginPosition(
      Id sequence_id) const noexcept;

  /// Returns sequence end position.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Optional end position in beats.
  [[nodiscard]] StatusOr<double> GetSequenceEndPosition(
      Id sequence_id) const noexcept;

  /// Returns sequence loop begin offset.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Loop begin offset in beats.
  [[nodiscard]] StatusOr<double> GetSequenceLoopBeginOffset(
      Id sequence_id) const noexcept;

  /// Returns sequence loop length.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Loop length in beats.
  [[nodiscard]] StatusOr<double> GetSequenceLoopLength(
      Id sequence_id) const noexcept;

  /// Returns the playback position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPlaybackPosition() const noexcept;

  /// Returns the playback tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetPlaybackTempo() const noexcept;

  /// Returns whether instrument is muted or not.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if muted, or false if not, or error status.
  [[nodiscard]] StatusOr<bool> IsInstrumentMuted(
      Id instrument_id) const noexcept;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param pitch Note pitch.
  /// @return True if note is active, or false if not, or error status.
  [[nodiscard]] StatusOr<bool> IsInstrumentNoteOn(Id instrument_id,
                                                  float pitch) const noexcept;

  /// Returns whether the sequence is empty or not.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return True if empty (i.e., has no notes), false if not, or error status.
  [[nodiscard]] StatusOr<bool> IsSequenceEmpty(Id sequence_id) const noexcept;

  /// Returns whether the sequence is looping or not.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return True if looping, false if not, or error status.
  [[nodiscard]] StatusOr<bool> IsSequenceLooping(Id sequence_id) const noexcept;

  /// Returns whether the playback is currently active or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

  /// Processes next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return Status.
  Status ProcessInstrument(Id instrument_id, double timestamp, float* output,
                           int num_channels, int num_frames) noexcept;

  /// Removes all sequence notes.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Status.
  Status RemoveAllSequenceNotes(Id sequence_id) noexcept;

  /// Removes all sequence notes at range.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllSequenceNotes(Id sequence_id, double begin_position,
                                double end_position) noexcept;

  /// Removes sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Status.
  Status RemoveSequence(Id sequence_id) noexcept;

  /// Removes sequence note.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param note_id Note id.
  /// @return Status.
  Status RemoveSequenceNote(Id sequence_id, Id note_id) noexcept;

  /// Resets all instrument parameters to default value.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status ResetAllInstrumentParameters(Id instrument_id) noexcept;

  /// Resets instrument parameter to default value.
  ///
  /// @param instrument_id Instrument id.
  /// @param index Parameter index.
  /// @return Status.
  Status ResetInstrumentParameter(Id instrument_id, int index) noexcept;

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

  /// Sets instrument note off callback.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_off_callback Note off callback.
  /// @param user_data User data.
  Status SetInstrumentNoteOffCallback(
      Id instrument_id, BarelyInstrument_NoteOffCallback note_off_callback,
      void* user_data) noexcept;

  /// Sets instrument note on callback.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_on_callback Note on callback.
  /// @param user_data User data.
  Status SetInstrumentNoteOnCallback(
      Id instrument_id, BarelyInstrument_NoteOnCallback note_on_callback,
      void* user_data) noexcept;

  /// Sets instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetInstrumentParameter(Id instrument_id, int index,
                                float value) noexcept;

  /// Sets sequence begin offset.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetSequenceBeginOffset(Id sequence_id, double begin_offset) noexcept;

  /// Sets sequence begin position.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetSequenceBeginPosition(Id sequence_id,
                                  double begin_position) noexcept;

  /// Sets sequence end position.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetSequenceEndPosition(Id sequence_id, double end_position) noexcept;

  /// Sets sequence instrument.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetSequenceInstrument(Id sequence_id, Id instrument_id) noexcept;

  /// Sets whether sequence should be looping or not.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param loop True if looping.
  /// @return Status.
  Status SetSequenceLoop(Id sequence_id, bool loop) noexcept;

  /// Sets sequence loop begin offset.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetSequenceLoopBeginOffset(Id sequence_id,
                                    double loop_begin_offset) noexcept;

  /// Sets sequence loop length.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetSequenceLoopLength(Id sequence_id, double loop_length) noexcept;

  /// Sets the playback beat callback.
  ///
  /// @param beat_callback Beat callback.
  void SetPlaybackBeatCallback(BeatCallback beat_callback) noexcept;

  /// Sets the playback position.
  ///
  /// @param position Position in beats.
  void SetPlaybackPosition(double position) noexcept;

  /// Sets the playback tempo.
  ///
  /// @param tempo Tempo in bpm.
  void SetPlaybackTempo(double tempo) noexcept;

  /// Starts instrument note.
  ///
  /// @param instrument_id Instrument id.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartInstrumentNote(Id instrument_id, float pitch,
                             float intensity) noexcept;

  /// Starts the playback.
  void StartPlayback() noexcept;

  /// Stop all instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status StopAllInstrumentNotes(Id instrument_id) noexcept;

  /// Stops instrument note.
  ///
  /// @param instrument_id Instrument id.
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopInstrumentNote(Id instrument_id, float pitch) noexcept;

  /// Stops the playback.
  void StopPlayback() noexcept;

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

  // TODO: Temp until engine is removed.
  Transport& transport() { return transport_; }

 private:
  // Monotonic identifier counter.
  Id id_counter_ = 0;

  // Instrument controller by id map.
  std::unordered_map<Id, InstrumentController> controllers_;

  // Instrument controller by id map.
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Audio thread task runner.
  TaskRunner runner_;

  // List of sequences.
  std::unordered_map<Id, Sequence> sequences_;

  // Playback tempo in bpm.
  double playback_tempo_;

  // Playback transport.
  Transport transport_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
