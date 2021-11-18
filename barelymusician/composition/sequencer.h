#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_

#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
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
    if (sequences_.emplace(sequence_id, std::pair{NoteSequence{}, std::nullopt})
            .second) {
      return Status::kOk;
    }
    return Status::kAlreadyExists;
  }

  // TDOO: this should not exist?
  StatusOr<NoteSequence*> GetSequence(Id sequence_id) {
    if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
      return &sequence->first;
    }
    return Status::kNotFound;
  }

  Status SetInstrument(Id sequence_id, std::optional<Id> instrument_id) {
    if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
      sequence->second = instrument_id;
      return Status::kOk;
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
      const double note_end_position = note_event.end_position;
      if (note_end_position < end_position ||
          begin_position < note_begin_position) {
        // TODO: Make this check more properly.
        const double off_position = (note_end_position < end_position)
                                        ? note_end_position
                                        : begin_position;
        events.emplace(get_timestamp_fn(off_position),
                       std::pair{note_event.instrument_id,
                                 SetNoteOffEvent{note_event.pitch}});
        it = active_notes_.erase(it);
      } else {
        ++it;
      }
    }
    // Process sequence events.
    for (const auto& [sequence_id, sequence_instrument_id_pair] : sequences_) {
      if (!sequence_instrument_id_pair.second) continue;
      const auto& sequence = sequence_instrument_id_pair.first;
      const Id instrument_id = *sequence_instrument_id_pair.second;
      if (!instrument_id) continue;
      sequence.Process(
          begin_position, end_position, [&](double position, const Note& note) {
            // TODO: GetPitch(note.pitch);
            const float pitch = note.pitch;
            // TODO: GetIntensity(note.intensity);
            const float intensity = note.intensity;
            events.emplace(
                get_timestamp_fn(position),
                std::pair{instrument_id, SetNoteOnEvent{pitch, intensity}});
            // TODO: GetDuration(note.duration);
            const double duration = note.duration;
            const double off_position = position + duration;
            if (off_position < end_position) {
              events.emplace(get_timestamp_fn(off_position),
                             std::pair{instrument_id, SetNoteOffEvent{pitch}});
            } else {
              active_notes_.emplace(
                  position, NoteEvent{instrument_id, off_position, pitch});
            }
          });
    }
    return events;
  }

 private:
  struct NoteEvent {
    Id instrument_id;
    double end_position;
    float pitch;
  };
  std::multimap<double, NoteEvent> active_notes_;
  std::unordered_map<Id, std::pair<NoteSequence, std::optional<Id>>> sequences_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCER_H_
