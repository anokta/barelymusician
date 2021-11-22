#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <algorithm>
#include <map>
#include <unordered_set>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
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
  InstrumentControllerEvents Perform(double begin_position, double end_position,
                                     Conductor& conductor) {
    InstrumentControllerEvents events;
    // Perform note off events.
    for (auto it = active_notes_.begin(); it != active_notes_.end();) {
      const auto& [note_begin_position, active_note] = *it;
      double note_end_position = begin_position;
      const auto duration_or =
          conductor.TransformNoteDuration(active_note.duration);
      if (IsOk(duration_or)) {
        note_end_position = note_begin_position + GetStatusOrValue(duration_or);
        if (note_end_position < end_position) {
          note_end_position = std::max(begin_position, note_end_position);
        } else if (begin_position < note_begin_position) {
          note_end_position = begin_position;
        } else {
          ++it;
          continue;
        }
      } else {
        DLOG(ERROR) << "Conductor failed to transform note duration: "
                    << ToString(duration_or);
      }
      events.emplace(note_end_position,
                     std::pair{active_note.instrument_id,
                               SetNoteOffEvent{active_note.pitch}});
      it = active_notes_.erase(it);
    }
    // Perform score events.
    if (!instrument_ids_.empty()) {
      score_.Process(
          begin_position, end_position, [&](double position, const Note& note) {
            const auto pitch_or = conductor.TransformNotePitch(note.pitch);
            if (!IsOk(pitch_or)) {
              DLOG(ERROR) << "Conductor failed to transform note pitch: "
                          << ToString(pitch_or);
              return;
            }
            const float pitch = GetStatusOrValue(pitch_or);

            const auto intensity_or =
                conductor.TransformNoteIntensity(note.intensity);
            if (!IsOk(intensity_or)) {
              DLOG(ERROR) << "Conductor failed to transform note intensity: "
                          << ToString(intensity_or);
              return;
            }
            const float intensity = GetStatusOrValue(intensity_or);

            const auto duration_or =
                conductor.TransformNoteDuration(note.duration);
            if (!IsOk(duration_or)) {
              DLOG(ERROR) << "Conductor failed to transform note duration: "
                          << ToString(duration_or);
              return;
            }
            const double note_end_position =
                position + GetStatusOrValue(duration_or);

            for (const auto& instrument_id : instrument_ids_) {
              // Perform note on event.
              events.emplace(
                  position,
                  std::pair{instrument_id, SetNoteOnEvent{pitch, intensity}});
              // Perform note off event.
              if (note_end_position < end_position) {
                events.emplace(
                    note_end_position,
                    std::pair{instrument_id, SetNoteOffEvent{pitch}});
              } else {
                active_notes_.emplace(
                    position, ActiveNote{instrument_id, note.duration, pitch});
              }
            }
          });
    }
    return events;
  }

  // TODO: wip
  Status AddInstrument(Id instrument_id) {
    if (instrument_ids_.emplace(instrument_id).second) {
      return Status::kOk;
    }
    return Status::kAlreadyExists;
  }

  // TODO: wip
  NoteSequence& GetScore() { return score_; }

  // TODO: wip
  void Stop() { active_notes_.clear(); }

 private:
  // Active note that's being performed.
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

  // Score to perform.
  NoteSequence score_;

  // List of instrument ids to perform.
  std::unordered_set<Id> instrument_ids_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
