#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Performer::Performer() {}

Status Performer::AddInstrument(Id instrument_id) {
  if (instrument_ids_.emplace(instrument_id).second) {
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

InstrumentControllerEventPairs Performer::Perform(Conductor& conductor,
                                                  double begin_position,
                                                  double end_position) {
  InstrumentControllerEventPairs event_pairs;
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
    event_pairs.emplace(
        note_end_position,
        InstrumentControllerEventPair{active_note.instrument_id,
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
            event_pairs.emplace(
                position, InstrumentControllerEventPair{
                              instrument_id, SetNoteOnEvent{pitch, intensity}});
            // Perform note off event.
            if (note_end_position < end_position) {
              event_pairs.emplace(note_end_position,
                                  InstrumentControllerEventPair{
                                      instrument_id, SetNoteOffEvent{pitch}});
            } else {
              active_notes_.emplace(
                  position, ActiveNote{instrument_id, note.duration, pitch});
            }
          }
        });
  }
  return event_pairs;
}

std::vector<InstrumentControllerEventPair> Performer::RemoveAllInstruments() {
  std::vector<InstrumentControllerEventPair> event_pairs;
  event_pairs.reserve(active_notes_.size());
  for (const auto& [position, active_note] : active_notes_) {
    event_pairs.emplace_back(active_note.instrument_id,
                             SetNoteOffEvent{active_note.pitch});
  }
  return event_pairs;
}

StatusOr<std::vector<InstrumentControllerEvent>> Performer::RemoveInstrument(
    Id instrument_id) {
  if (instrument_ids_.erase(instrument_id) > 0) {
    std::vector<InstrumentControllerEvent> events;
    for (auto it = active_notes_.begin(); it != active_notes_.end();) {
      if (it->second.instrument_id == instrument_id) {
        events.push_back(SetNoteOffEvent{it->second.pitch});
        it = active_notes_.erase(it);
      } else {
        ++it;
      }
    }
    return events;
  }
  return Status::kNotFound;
}

}  // namespace barelyapi
