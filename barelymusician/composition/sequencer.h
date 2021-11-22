#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/conductor.h"
#include "barelymusician/composition/conductor_definition.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_sequence.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

/// Playback sequencer.
class Sequencer {
 public:
  Status CreateSequence(Id sequence_id) {
    if (sequence_id == kInvalidId) return Status::kInvalidArgument;
    if (sequences_
            .emplace(sequence_id,
                     std::pair{NoteSequence{}, std::unordered_set<Id>{}})
            .second) {
      return Status::kOk;
    }
    return Status::kAlreadyExists;
  }

  Status DestroySequence(Id sequence_id) {
    if (sequences_.erase(sequence_id) > 0) {
      return Status::kOk;
    }
    return Status::kNotFound;
  }

  // TDOO: this should not exist?
  StatusOr<NoteSequence*> GetSequence(Id sequence_id) {
    if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
      return &sequence->first;
    }
    return Status::kNotFound;
  }

  Status AddInstrument(Id sequence_id, Id instrument_id) {
    if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
      if (sequence->second.emplace(instrument_id).second) {
        return Status::kOk;
      }
      return Status::kAlreadyExists;
    }
    return Status::kNotFound;
  }

  // TODO: this should not exist?
  void Stop() { active_notes_.clear(); }

  InstrumentControllerEvents Process(
      double begin_position, double end_position,
      const Transport::GetTimestampFn& get_timestamp_fn) {
    InstrumentControllerEvents events;
    // Process note off events.
    for (auto it = active_notes_.begin(); it != active_notes_.end();) {
      const double note_begin_position = it->first;
      const auto& note_event = it->second;
      double note_end_position = note_event.end_position;
      if (note_end_position < end_position) {
        note_end_position = std::max(begin_position, note_end_position);
      } else if (begin_position < note_begin_position) {
        note_end_position = begin_position;
      } else {
        ++it;
        continue;
      }
      events.emplace(get_timestamp_fn(note_end_position),
                     std::pair{note_event.instrument_id,
                               SetNoteOffEvent{note_event.pitch}});
      it = active_notes_.erase(it);
    }
    // Process sequence events.
    for (const auto& [sequence_id, sequence_instrument_id_pair] : sequences_) {
      const auto& instrument_ids = sequence_instrument_id_pair.second;
      if (instrument_ids.empty()) continue;
      const auto& sequence = sequence_instrument_id_pair.first;
      sequence.Process(
          begin_position, end_position, [&](double position, const Note& note) {
            const auto pitch_or = conductor_.TransformNotePitch(note.pitch);
            if (!IsOk(pitch_or)) return;
            const float pitch = GetStatusOrValue(pitch_or);

            const auto intensity_or =
                conductor_.TransformNoteIntensity(note.intensity);
            if (!IsOk(intensity_or)) return;
            const float intensity = GetStatusOrValue(intensity_or);

            for (const auto& instrument_id : instrument_ids) {
              events.emplace(
                  get_timestamp_fn(position),
                  std::pair{instrument_id, SetNoteOnEvent{pitch, intensity}});
            }

            const auto duration_or =
                conductor_.TransformNoteDuration(note.duration);
            if (!IsOk(duration_or)) return;
            const double duration = GetStatusOrValue(duration_or);

            const double note_end_position = position + duration;
            if (note_end_position < end_position) {
              for (const auto& instrument_id : instrument_ids) {
                events.emplace(
                    get_timestamp_fn(note_end_position),
                    std::pair{instrument_id, SetNoteOffEvent{pitch}});
              }
            } else {
              for (const auto& instrument_id : instrument_ids) {
                active_notes_.emplace(
                    position,
                    NoteEvent{instrument_id, note_end_position, pitch});
              }
            }
          });
    }
    return events;
  }

  void SetConductor(ConductorDefinition definition) {
    conductor_ = Conductor(std::move(definition));
  }

 private:
  struct NoteEvent {
    Id instrument_id;
    double end_position;
    float pitch;
  };
  std::multimap<double, NoteEvent> active_notes_;
  std::unordered_map<Id, std::pair<NoteSequence, std::unordered_set<Id>>>
      sequences_;

  Conductor conductor_{ConductorDefinition{}};
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
