#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

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
#include "barelymusician/engine/instrument_param_definition.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

// BarelyMusician C++ API.
class BarelyMusician {
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

  /// Constructs new |BarelyMusician|.
  ///
  /// @param sample_rate System sampling rate in Hz.
  explicit BarelyMusician(int sample_rate);

  /// Adds new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  Id AddInstrument(InstrumentDefinition definition,
                   InstrumentParamDefinitions param_definitions);

  /// Adds new performer.
  ///
  /// @return Performer id.
  Id AddPerformer();

  /// Adds performer instrument.
  ///
  /// @param performer_id Performer id.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status AddPerformerInstrument(Id performer_id, Id instrument_id);

  /// Adds performer note.
  ///
  /// @param performer_id Performer id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Note id.
  StatusOr<Id> AddPerformerNote(Id performer_id, double position, Note note);

  /// Returns performer begin offset.
  ///
  /// @param performer_id Performer id.
  /// @return Begin offset in beats.
  StatusOr<double> GetPerformerBeginOffset(Id performer_id) const;

  /// Returns performer begin position.
  ///
  /// @param performer_id Performer id.
  /// @return Optional begin position in beats.
  StatusOr<std::optional<double>> GetPerformerBeginPosition(
      Id performer_id) const;

  /// Returns performer end position.
  ///
  /// @param performer_id Performer id.
  /// @return Optional end position in beats.
  StatusOr<std::optional<double>> GetPerformerEndPosition(
      Id performer_id) const;

  /// Returns performer loop begin offset.
  ///
  /// @param performer_id Performer id.
  /// @return Loop begin offset in beats.
  StatusOr<double> GetPerformerLoopBeginOffset(Id performer_id) const;

  /// Returns performer loop length.
  ///
  /// @param performer_id Performer id.
  /// @return Loop length in beats.
  StatusOr<double> GetPerformerLoopLength(Id performer_id) const;

  /// Returns the playback position.
  ///
  /// @return Position in beats.
  double GetPlaybackPosition() const;

  /// Returns the playback tempo.
  ///
  /// @return Tempo in BPM.
  double GetPlaybackTempo() const;

  /// Returns whether the performer is empty or not.
  ///
  /// @param performer_id Performer id.
  /// @return True if empty (i.e., has no notes), false if not, or error status.
  StatusOr<bool> IsPerformerEmpty(Id performer_id) const;

  /// Returns whether the performer is looping or not.
  ///
  /// @param performer_id Performer id.
  /// @return True if looping, false if not, or error status.
  StatusOr<bool> IsPerformerLooping(Id performer_id) const;

  /// Returns whether the playback is currently active or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void ProcessInstrument(Id instrument_id, double timestamp, float* output,
                         int num_channels, int num_frames);

  /// Removes all performer instruments.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemoveAllPerformerInstruments(Id performer_id);

  /// Removes all performer notes.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemoveAllPerformerNotes(Id performer_id);

  /// Removes all performer notes at range.
  ///
  /// @param performer_id Performer id.
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                 double end_position);

  /// Removes instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status RemoveInstrument(Id instrument_id);

  /// Removes performer.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status RemovePerformer(Id performer_id);

  /// Removes performer instrument.
  ///
  /// @param performer_id Performer id.
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status RemovePerformerInstrument(Id performer_id, Id instrument_id);

  /// Removes performer note.
  ///
  /// @param performer_id Performer id.
  /// @param note_id Note id.
  /// @return Status.
  Status RemovePerformerNote(Id performer_id, Id note_id);

  /// Sets all notes of all instruments off.
  void SetAllInstrumentNotesOff();

  /// Sets all instrument notes off.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentNotesOff(Id instrument_id);

  /// Sets all parameters of all instruments to default.
  void SetAllInstrumentParamsToDefault();

  /// Sets all instrument parameters to default.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentParamsToDefault(Id instrument_id);

  /// Sets conductor.
  ///
  /// @param definition Conductor definition.
  void SetConductor(ConductorDefinition definition);

  // TODO(#49): Add the rest of the instrument setters.

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetInstrumentNoteOff(Id instrument_id, float note_pitch);

  /// Sets the instrument note off callback.
  ///
  /// @param instrument_note_off_callback Instrument note off callback.
  void SetInstrumentNoteOffCallback(
      InstrumentNoteOffCallback instrument_note_off_callback);

  /// Sets instrument note on.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                             float note_intensity);

  /// Sets the instrument note on callback.
  ///
  /// @param instrument_note_on_callback Instrument note on callback.
  void SetInstrumentNoteOnCallback(
      InstrumentNoteOnCallback instrument_note_on_callback);

  /// Sets instrument parameter.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetInstrumentParam(Id instrument_id, int param_id, float param_value);

  /// Sets instrument parameter to default.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Status.
  Status SetInstrumentParamToDefault(Id instrument_id, int param_id);

  /// Sets performer begin offset.
  ///
  /// @param performer_id Performer id.
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetPerformerBeginOffset(Id performer_id, double begin_offset);

  /// Sets performer begin position.
  ///
  /// @param performer_id Performer id.
  /// @param begin_position Optional begin position in beats.
  /// @return Status.
  Status SetPerformerBeginPosition(Id performer_id,
                                   std::optional<double> begin_position);

  /// Sets performer end position.
  ///
  /// @param performer_id Performer id.
  /// @param end_position Optional end position in beats.
  /// @return Status.
  Status SetPerformerEndPosition(Id performer_id,
                                 std::optional<double> end_position);

  /// Sets performer loop begin offset.
  ///
  /// @param performer_id Performer id.
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetPerformerLoopBeginOffset(Id performer_id, double loop_begin_offset);

  /// Sets performer loop length.
  ///
  /// @param performer_id Performer id.
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetPerformerLoopLength(Id performer_id, double loop_length);

  /// Sets performer looping.
  ///
  /// @param performer_id Performer id.
  /// @param looping True if looping.
  /// @return Status.
  Status SetPerformerLooping(Id performer_id, bool looping);

  /// Sets the playback beat callback.
  ///
  /// @param playback_beat_callback Playback beat callback.
  void SetPlaybackBeatCallback(PlaybackBeatCallback playback_beat_callback);

  /// Sets the playback position.
  ///
  /// @param position Position in beats.
  void SetPlaybackPosition(double position);

  /// Sets the playback tempo.
  ///
  /// @param tempo Tempo in BPM.
  void SetPlaybackTempo(double tempo);

  /// Sets the playback update callback.
  ///
  /// @param playback_update_callback Playback update callback.
  void SetPlaybackUpdateCallback(
      PlaybackUpdateCallback playback_update_callback);

  /// Starts the playback.
  void StartPlayback();

  /// Stops the playback.
  void StopPlayback();

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

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

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
