#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_sequence.h"
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

  /// Returns instrument events that are performed at given range.
  ///
  /// @param conductor Reference to conductor.
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Performed instrument event pairs.
  InstrumentControllerEventPairs Perform(Conductor& conductor,
                                         double begin_position,
                                         double end_position);

  /// Removes all instruments.
  ///
  /// @return List of instrument event-id pairs to be processed.
  std::vector<InstrumentControllerEventPair> RemoveAllInstruments();

  /// Removes instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of instrument events to be processed, or error status.
  StatusOr<std::vector<InstrumentControllerEvent>> RemoveInstrument(
      Id instrument_id);

  // TODO: wip
  NoteSequence& GetScore() { return score_; }

  // TODO: wip
  void Stop() { active_notes_.clear(); }

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

  // List of instrument ids to perform.
  std::unordered_set<Id> instrument_ids_;

  // Score to perform.
  NoteSequence score_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
