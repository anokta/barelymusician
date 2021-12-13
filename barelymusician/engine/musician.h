#ifndef BARELYMUSICIAN_ENGINE_MUSICIAN_H_
#define BARELYMUSICIAN_ENGINE_MUSICIAN_H_

#include <any>
#include <functional>
#include <optional>
#include <unordered_map>

#include "barelymusician/common/id.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/transport.h"

namespace barely {

// barely::Musician C++ API.
class Musician {
 public:
  /// Instrument note off callback signature.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  using InstrumentNoteOffCallback =
      std::function<void(Id instrument_id, float note_pitch)>;

  /// Instrument note on callback signature.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  using InstrumentNoteOnCallback = std::function<void(
      Id instrument_id, float note_pitch, float note_intensity)>;

  /// Playback beat callback signature.
  using PlaybackBeatCallback = Transport::BeatCallback;

  /// Playback update callback signature.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  using PlaybackUpdateCallback =
      std::function<void(double begin_position, double end_position)>;

  /// Constructs new |Musician|.
  ///
  /// @param sample_rate System sampling rate in Hz.
  explicit Musician(int sample_rate) noexcept;

  /// Adds new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  Id AddInstrument(InstrumentDefinition definition,
                   ParamDefinitionMap param_definitions) noexcept;

  /// Adds new performer.
  ///
  /// @return Performer id.
  Id AddPerformer() noexcept;

  /// Adds performer instrument.
  ///
  /// @param performer_id Performer id.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status AddPerformerInstrument(Id performer_id, Id instrument_id) noexcept;

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
  /// @return Tempo in BPM.
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
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void ProcessInstrument(Id instrument_id, double timestamp, float* output,
                         int num_channels, int num_frames) noexcept;

  /// Removes all performer instruments.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemoveAllPerformerInstruments(Id performer_id) noexcept;

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

  /// Removes performer instrument.
  ///
  /// @param performer_id Performer id.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status RemovePerformerInstrument(Id performer_id, Id instrument_id) noexcept;

  /// Removes performer note.
  ///
  /// @param performer_id Performer id.
  /// @param note_id Note id.
  /// @return Status.
  Status RemovePerformerNote(Id performer_id, Id note_id) noexcept;

  /// Sets all notes of all instruments off.
  void SetAllInstrumentNotesOff() noexcept;

  /// Sets all instrument notes off.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentNotesOff(Id instrument_id) noexcept;

  /// Sets all parameters of all instruments to default.
  void SetAllInstrumentParamsToDefault() noexcept;

  /// Sets all instrument parameters to default.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentParamsToDefault(Id instrument_id) noexcept;

  /// Sets conductor.
  ///
  /// @param definition Conductor definition.
  /// @param definition Conductor parameter definitions.
  void SetConductor(ConductorDefinition definition,
                    ParamDefinitionMap param_definitions) noexcept;

  /// Sets custom instrument data.
  ///
  /// @param instrument_id Instrument id.
  /// @param custom_data Custom data.
  /// @return Status.
  Status SetCustomInstrumentData(Id instrument_id,
                                 std::any custom_data) noexcept;

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetInstrumentNoteOff(Id instrument_id, float note_pitch) noexcept;

  /// Sets the instrument note off callback.
  ///
  /// @param instrument_note_off_callback Instrument note off callback.
  void SetInstrumentNoteOffCallback(
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
  /// @param instrument_note_on_callback Instrument note on callback.
  void SetInstrumentNoteOnCallback(
      InstrumentNoteOnCallback instrument_note_on_callback) noexcept;

  /// Sets instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetInstrumentParam(Id instrument_id, int param_id,
                            float param_value) noexcept;

  /// Sets instrument parameter to default.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Status.
  Status SetInstrumentParamToDefault(Id instrument_id, int param_id) noexcept;

  /// Sets performer begin offset.
  ///
  /// @param performer_id Performer id.
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetPerformerBeginOffset(Id performer_id, double begin_offset) noexcept;

  /// Sets performer begin position.
  ///
  /// @param performer_id Performer id.
  /// @param begin_position Optional begin position in beats.
  /// @return Status.
  Status SetPerformerBeginPosition(
      Id performer_id, std::optional<double> begin_position) noexcept;

  /// Sets performer end position.
  ///
  /// @param performer_id Performer id.
  /// @param end_position Optional end position in beats.
  /// @return Status.
  Status SetPerformerEndPosition(Id performer_id,
                                 std::optional<double> end_position) noexcept;

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
  /// @param playback_beat_callback Playback beat callback.
  void SetPlaybackBeatCallback(
      PlaybackBeatCallback playback_beat_callback) noexcept;

  /// Sets the playback position.
  ///
  /// @param position Position in beats.
  void SetPlaybackPosition(double position) noexcept;

  /// Sets the playback tempo.
  ///
  /// @param tempo Tempo in BPM.
  void SetPlaybackTempo(double tempo) noexcept;

  /// Sets the playback update callback.
  ///
  /// @param playback_update_callback Playback update callback.
  void SetPlaybackUpdateCallback(
      PlaybackUpdateCallback playback_update_callback) noexcept;

  /// Sets the sample rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
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
  // Conductor.
  Conductor conductor_;

  // Id generator.
  IdGenerator id_generator_;

  // Instrument manager.
  InstrumentManager instrument_manager_;

  // Instrument note off callback.
  InstrumentNoteOffCallback instrument_note_off_callback_;

  // Instrument note on callback.
  InstrumentNoteOnCallback instrument_note_on_callback_;

  // List of performers.
  std::unordered_map<Id, Performer> performers_;

  // Playback tempo in BPM.
  double playback_tempo_;

  // Playback update callback.
  PlaybackUpdateCallback playback_update_callback_;

  // Playback transport.
  Transport transport_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_MUSICIAN_H_
