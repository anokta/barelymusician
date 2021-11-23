#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <optional>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Performer::Performer()
    : begin_position_(std::nullopt), end_position_(std::nullopt) {}

Status Performer::AddInstrument(Id instrument_id) {
  if (instrument_ids_.emplace(instrument_id).second) {
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

void Performer::ClearAllActiveNotes() { active_notes_.clear(); }

std::optional<double> Performer::GetBeginPosition() const {
  return begin_position_;
}

std::optional<double> Performer::GetEndPosition() const {
  return end_position_;
}

Sequence* Performer::GetMutableSequence() { return &sequence_; }

const Sequence& Performer::GetSequence() const { return sequence_; }

InstrumentIdEventPairs Performer::Perform(double begin_position,
                                          double end_position,
                                          Conductor& conductor) {
  InstrumentIdEventPairs id_event_pairs;
  // Perform note off events.
  // TODO: Handle begin/end positions.
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
    id_event_pairs.emplace(
        note_end_position,
        InstrumentIdEventPair{active_note.instrument_id,
                              SetNoteOffEvent{active_note.pitch}});
    it = active_notes_.erase(it);
  }
  // Perform score events.
  // TODO: Refactor better for begin/end position.
  if (!instrument_ids_.empty() && !sequence_.IsEmpty()) {
    if (end_position_) {
      end_position = std::min(end_position, *end_position_);
    }
    if (begin_position_) {
      begin_position =
          std::max(begin_position, *begin_position_) - *begin_position_;
      end_position -= *begin_position_;
    }
    if (begin_position < end_position) {
      const double position_offset = begin_position_ ? *begin_position_ : 0.0;
      sequence_.Process(
          begin_position, end_position, [&](double position, const Note& note) {
            position += position_offset;

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
              id_event_pairs.emplace(
                  position,
                  InstrumentIdEventPair{instrument_id,
                                        SetNoteOnEvent{pitch, intensity}});
              // Perform note off event.
              if (note_end_position < end_position) {
                id_event_pairs.emplace(
                    note_end_position,
                    InstrumentIdEventPair{instrument_id,
                                          SetNoteOffEvent{pitch}});
              } else {
                active_notes_.emplace(
                    position, ActiveNote{instrument_id, note.duration, pitch});
              }
            }
          });
    }
  }
  return id_event_pairs;
}

std::vector<InstrumentIdEventPair> Performer::RemoveAllInstruments() {
  std::vector<InstrumentIdEventPair> id_event_pairs;
  id_event_pairs.reserve(active_notes_.size());
  for (const auto& [position, active_note] : active_notes_) {
    id_event_pairs.emplace_back(active_note.instrument_id,
                                SetNoteOffEvent{active_note.pitch});
  }
  return id_event_pairs;
}

StatusOr<std::vector<InstrumentEvent>> Performer::RemoveInstrument(
    Id instrument_id) {
  if (instrument_ids_.erase(instrument_id) > 0) {
    std::vector<InstrumentEvent> events;
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

void Performer::SetBeginPosition(std::optional<double> begin_position) {
  begin_position_ = std::move(begin_position);
}

void Performer::SetEndPosition(std::optional<double> end_position) {
  end_position_ = std::move(end_position);
}

}  // namespace barelyapi
