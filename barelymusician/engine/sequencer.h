#ifndef BARELYMUSICIAN_ENGINE_SEQUENCER_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCER_H_

#include <map>
#include <unordered_map>

#include "barelymusician/common/id.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

/// Playback sequencer.
class Sequencer {
 public:
  /// Beat callback signature.
  using BeatCallback = Transport::BeatCallback;

  explicit Sequencer(InstrumentManager* manager);

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

  /// Removes all scheduled notes of all instruments.
  void RemoveAllScheduledInstrumentNotes();

  /// Removes all scheduled instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  void RemoveAllScheduledInstrumentNotes(Id instrument_id);

  /// Schedules instrument note.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_begin_position Note begin position in beats.
  /// @param note_end_position Note end position in beats.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return True if successful, false otherwise.
  void ScheduleInstrumentNote(Id instrument_id, double note_begin_position,
                              double note_end_position, float note_pitch,
                              float note_intensity);

  /// Sets the beat callback.
  ///
  /// @param beat_callback Playback beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

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

  // Stops the playback.
  void StopPlayback();

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  void StopAllNotes();

  struct Track {
    std::multimap<double, InstrumentEvent> events;
  };

  std::unordered_map<Id, Track> tracks_;

  // Playback transport.
  Transport transport_;

  // Instrument manager.
  InstrumentManager* manager_;  // not owned.
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCER_H_
