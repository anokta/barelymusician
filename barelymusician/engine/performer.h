#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <unordered_set>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

/// Class that performs a musical sequence with a set of instruments.
class Performer {
 public:
  /// Constructs new `Performer`.
  Performer() noexcept;

  /// Clears all active notes.
  void ClearAllActiveNotes() noexcept;

  /// Returns instrument.
  ///
  /// @return Instrument identifier.
  Id GetInstrument() const noexcept;

  /// Returns mutable sequence.
  ///
  /// @return Mutable sequence.
  Sequence* GetMutableSequence() noexcept;

  /// Returns sequence.
  ///
  /// @return Sequence.
  const Sequence& GetSequence() const noexcept;

  /// Returns sequence begin position.
  ///
  /// @return Optional sequence begin position in beats.
  double GetSequenceBeginPosition() const noexcept;

  /// Returns sequence end position.
  ///
  /// @return Optional sequence end position in beats.
  double GetSequenceEndPosition() const noexcept;

  /// Performs instruments at given range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @param conductor Conductor.
  /// @return Instrument id-event pairs to be processed.
  InstrumentIdEventPairMap Perform(double begin_position, double end_position,
                                   Conductor& conductor) noexcept;

  /// Sets instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of instrument events.
  std::vector<InstrumentEvent> SetInstrument(Id instrument_id) noexcept;

  /// Sets sequence begin position.
  ///
  /// @param sequence_begin_position Optional sequence begin position in beats.
  void SetSequenceBeginPosition(double sequence_begin_position) noexcept;

  /// Sets sequence end position.
  ///
  /// @param sequence_end_position Optional sequence end position in beats.
  void SetSequenceEndPosition(double sequence_end_position) noexcept;

 private:
  // Active note that is being performed.
  struct ActiveNote {
    // Note duration.
    NoteDuration duration;

    // Note pitch.
    float pitch;
  };

  // List of active notes.
  std::multimap<double, ActiveNote> active_notes_;

  // Instrument id to perform.
  Id instrument_id_;

  // Sequence to perform.
  Sequence sequence_;

  // Optional sequence begin position in beats.
  double sequence_begin_position_;

  // Optional sequence end position in beats.
  double sequence_end_position_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
