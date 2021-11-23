#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

#include <functional>
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

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  Id CreateInstrument(InstrumentDefinition definition,
                      InstrumentParamDefinitions param_definitions);

  /// Creates new performer.
  ///
  /// @return Performer id.
  Id CreatePerformer();

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status DestroyInstrument(Id instrument_id);

  /// Destroys performer.
  ///
  /// @param performer_id Performer id.
  /// @return Status.
  Status DestroyPerformer(Id performer_id);

  /// Returns the playback position.
  ///
  /// @return Position in beats.
  double GetPlaybackPosition() const;

  /// Returns the playback tempo.
  ///
  /// @return Tempo in BPM.
  double GetPlaybackTempo() const;

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
