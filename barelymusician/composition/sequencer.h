#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_

#include <unordered_map>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/performer.h"

namespace barelyapi {

/// Playback sequencer.
class Sequencer {
 public:
  Status CreateSequence(Id sequence_id) {
    if (sequence_id == kInvalidId) return Status::kInvalidArgument;
    if (performers_.emplace(sequence_id, Performer{}).second) {
      return Status::kOk;
    }
    return Status::kAlreadyExists;
  }

  Status DestroySequence(Id sequence_id) {
    if (performers_.erase(sequence_id) > 0) {
      return Status::kOk;
    }
    return Status::kNotFound;
  }

  // TDOO: this should not exist?
  StatusOr<NoteSequence*> GetSequence(Id sequence_id) {
    if (auto* sequence = FindOrNull(performers_, sequence_id)) {
      return &sequence->GetScore();
    }
    return Status::kNotFound;
  }

  Status AddInstrument(Id sequence_id, Id instrument_id) {
    if (auto* sequence = FindOrNull(performers_, sequence_id)) {
      return sequence->AddInstrument(instrument_id);
    }
    return Status::kNotFound;
  }

  // TODO: this should not exist?
  void Stop() {
    for (auto& [id, performer] : performers_) {
      performer.Stop();
    }
  }

  InstrumentControllerEvents Process(double begin_position,
                                     double end_position) {
    InstrumentControllerEvents events;
    for (auto& [id, performer] : performers_) {
      events.merge(performer.Perform(begin_position, end_position, conductor_));
    }
    return events;
  }

  double GetPlaybackTempo(double tempo) {
    return conductor_.TransformPlaybackTempo(60.0 * tempo) / 60.0;
  }

  void SetConductor(ConductorDefinition definition) {
    conductor_ = Conductor(std::move(definition));
  }

 private:
  std::unordered_map<Id, Performer> performers_;

  Conductor conductor_{ConductorDefinition{}};
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
