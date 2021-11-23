#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <optional>
#include <unordered_set>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

/// Class that performs a musical sequence with a set of instruments.
class Performer {
 public:
  /// Constructs new |Performer|.
  Performer();

  /// Adds instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status AddInstrument(Id instrument_id);

  /// Clears all active notes.
  void ClearAllActiveNotes();

  /// Returns mutable sequence.
  ///
  /// @return Mutable sequence.
  Sequence* GetMutableSequence();

  /// Returns sequence.
  ///
  /// @return Sequence.
  const Sequence& GetSequence() const;

  /// Returns sequence begin position.
  ///
  /// @return Optional sequence begin position in beats.
  std::optional<double> GetSequenceBeginPosition() const;

  /// Returns sequence end position.
  ///
  /// @return Optional sequence end position in beats.
  std::optional<double> GetSequenceEndPosition() const;

  /// Performs instruments at given range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @param conductor Conductor.
  /// @return Instrument id-event pairs to be processed.
  InstrumentIdEventPairs Perform(double begin_position, double end_position,
                                 Conductor& conductor);

  /// Removes all instruments.
  ///
  /// @return Instrument id-event pairs to be processed.
  std::vector<InstrumentIdEventPair> RemoveAllInstruments();

  /// Removes instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Instrument events to be processed, or error status.
  StatusOr<std::vector<InstrumentEvent>> RemoveInstrument(Id instrument_id);

  /// Sets sequence begin position.
  ///
  /// @param sequence_begin_position Optional sequence begin position in beats.
  void SetSequenceBeginPosition(std::optional<double> sequence_begin_position);

  /// Sets sequence end position.
  ///
  /// @param sequence_end_position Optional sequence end position in beats.
  void SetSequenceEndPosition(std::optional<double> sequence_end_position);

 private:
  // Active note that is being performed.
  struct ActiveNote {
    // Instrument id.
    Id instrument_id;

    // Note duration.
    NoteDuration duration;

    // Note pitch.
    float pitch;
  };

  // Returns |begin_position| clamped with |sequence_begin_position_|.
  double GetClampedBeginPosition(double begin_position);

  // Returns |end_position| clamped with |sequence_end_position_|.
  double GetClampedEndPosition(double end_position);

  // List of active notes.
  std::multimap<double, ActiveNote> active_notes_;

  // List of instrument ids to perform.
  std::unordered_set<Id> instrument_ids_;

  // Sequence to perform.
  Sequence sequence_;

  // Optional sequence begin position in beats.
  std::optional<double> sequence_begin_position_;

  // Optional sequence end position in beats.
  std::optional<double> sequence_end_position_;

  // Sequence position offset in beats.
  double sequence_position_offset_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
