#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_

#include <unordered_map>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/sequence.h"
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
  StatusOr<Sequence*> GetSequence(Id sequence_id) {
    if (auto* sequence = FindOrNull(performers_, sequence_id)) {
      return sequence->GetMutableSequence();
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
      performer.ClearAllActiveNotes();
    }
  }

  InstrumentIdEventPairs Process(double begin_position, double end_position) {
    InstrumentIdEventPairs id_event_pairs;
    for (auto& [id, performer] : performers_) {
      id_event_pairs.merge(
          performer.Perform(begin_position, end_position, conductor_));
    }
    return id_event_pairs;
  }

  void SetConductor(ConductorDefinition definition) {
    conductor_ = Conductor(std::move(definition));
  }

 private:
  std::unordered_map<Id, Performer> performers_;

  Conductor conductor_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
