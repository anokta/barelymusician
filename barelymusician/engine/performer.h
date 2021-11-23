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

/// Class that performs a musical score with a set of instruments.
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

  /// Returns the begin position.
  ///
  /// @return Optional begin position.
  std::optional<double> GetBeginPosition() const;

  /// Returns the end position.
  ///
  /// @return Optional end position.
  std::optional<double> GetEndPosition() const;

  /// Returns mutable sequence.
  ///
  /// @return Mutable sequence.
  Sequence* GetMutableSequence();

  /// Returns sequence.
  ///
  /// @return Sequence.
  const Sequence& GetSequence() const;

  /// Performs instrument events at given range.
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

  /// Sets begin position.
  ///
  /// @param begin_position Optional begin position.
  void SetBeginPosition(std::optional<double> begin_position);

  /// Sets end position.
  ///
  /// @param end_position Optional end position.
  void SetEndPosition(std::optional<double> end_position);

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

  // List of active notes.
  std::multimap<double, ActiveNote> active_notes_;

  // Optional begin position.
  std::optional<double> begin_position_;

  // Optional end position.
  std::optional<double> end_position_;

  // List of instrument ids to perform.
  std::unordered_set<Id> instrument_ids_;

  // Sequence to perform.
  Sequence sequence_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
