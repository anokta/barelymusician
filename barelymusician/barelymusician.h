#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

#include "barelymusician/common/id.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/instrument_param_definition.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

// BarelyMusician C++ API.
class BarelyMusician {
 public:
  /// Playback beat callback signature.
  using PlaybackBeatCallback = Transport::BeatCallback;

  // TODO(#49): Add |PlaybackUpdateCallback|.
  // TODO(#49): Add |InstrumentNoteOffCallback|.
  // TODO(#49): Add |InstrumentNoteOnCallback|.

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

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status DestroyInstrument(Id instrument_id);

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

  // TODO(#49): Add the rest of the instrument setters.

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return Status.
  // TODO(#81): Use |NotePitch| instead of |float|?
  Status SetInstrumentNoteOff(Id instrument_id, float note_pitch);

  /// Sets instrument note on.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  // TODO(#81): Use |NotePitch| and |NoteIntensity| instead of |float|s?
  Status SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                             float note_intensity);

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

  /// Starts the playback.
  void StartPlayback();

  /// Stops the playback.
  void StopPlayback();

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Id generator.
  IdGenerator id_generator_;

  // Instrument manager.
  InstrumentManager instrument_manager_;

  // Playback transport.
  Transport transport_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
