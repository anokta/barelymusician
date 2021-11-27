#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <optional>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barely {

Performer::Performer() noexcept
    : sequence_begin_position_(std::nullopt),
      sequence_end_position_(std::nullopt),
      sequence_position_offset_(0.0) {}

Status Performer::AddInstrument(Id instrument_id) noexcept {
  if (instrument_ids_.emplace(instrument_id).second) {
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

void Performer::ClearAllActiveNotes() noexcept { active_notes_.clear(); }

std::optional<double> Performer::GetSequenceBeginPosition() const noexcept {
  return sequence_begin_position_;
}

std::optional<double> Performer::GetSequenceEndPosition() const noexcept {
  return sequence_end_position_;
}

Sequence* Performer::GetMutableSequence() noexcept { return &sequence_; }

const Sequence& Performer::GetSequence() const noexcept { return sequence_; }

InstrumentIdEventPairs Performer::Perform(double begin_position,
                                          double end_position,
                                          Conductor& conductor) noexcept {
  InstrumentIdEventPairs id_event_pairs;

  // Perform active note events.
  for (auto it = active_notes_.begin(); it != active_notes_.end();) {
    const auto& [note_begin_position, active_note] = *it;
    double note_end_position = begin_position;
    const auto duration_or =
        conductor.TransformNoteDuration(active_note.duration);
    if (IsOk(duration_or)) {
      note_end_position =
          ClampEndPosition(note_begin_position + GetStatusOrValue(duration_or));
      if (note_end_position < end_position) {
        note_end_position = std::max(begin_position, note_end_position);
      } else if (begin_position < ClampBeginPosition(note_begin_position)) {
        note_end_position = begin_position;
      } else {
        ++it;
        continue;
      }
    }
    // Perform note off event.
    id_event_pairs.emplace(
        note_end_position,
        InstrumentIdEventPair{active_note.instrument_id,
                              SetNoteOffEvent{active_note.pitch}});
    it = active_notes_.erase(it);
  }

  // Perform sequence events.
  begin_position = ClampBeginPosition(begin_position);
  end_position = ClampEndPosition(end_position);
  if (!instrument_ids_.empty() && !sequence_.IsEmpty() &&
      begin_position < end_position) {
    sequence_.Process(
        begin_position, end_position, sequence_position_offset_,
        [&](double position, const Note& note) noexcept {
          // Get pitch.
          const auto pitch_or = conductor.TransformNotePitch(note.pitch);
          if (!IsOk(pitch_or)) {
            return;
          }
          const float pitch = GetStatusOrValue(pitch_or);
          // Get intensity.
          const auto intensity_or =
              conductor.TransformNoteIntensity(note.intensity);
          if (!IsOk(intensity_or)) {
            return;
          }
          const float intensity = GetStatusOrValue(intensity_or);
          // Get duration.
          const auto duration_or =
              conductor.TransformNoteDuration(note.duration);
          if (!IsOk(duration_or)) {
            return;
          }
          const double note_end_position =
              position + std::max(GetStatusOrValue(duration_or), 0.0);

          for (const auto& instrument_id : instrument_ids_) {
            // Perform note on event.
            id_event_pairs.emplace(
                position, InstrumentIdEventPair{
                              instrument_id, SetNoteOnEvent{pitch, intensity}});
            // Perform note off event.
            if (note_end_position < end_position) {
              id_event_pairs.emplace(
                  note_end_position,
                  InstrumentIdEventPair{instrument_id, SetNoteOffEvent{pitch}});
            } else {
              active_notes_.emplace(
                  position, ActiveNote{instrument_id, note.duration, pitch});
            }
          }
        });
  }

  return id_event_pairs;
}

std::vector<InstrumentIdEventPair> Performer::RemoveAllInstruments() noexcept {
  instrument_ids_.clear();
  std::vector<InstrumentIdEventPair> id_event_pairs;
  if (!active_notes_.empty()) {
    id_event_pairs.reserve(active_notes_.size());
    for (const auto& [position, active_note] : active_notes_) {
      id_event_pairs.emplace_back(active_note.instrument_id,
                                  SetNoteOffEvent{active_note.pitch});
    }
  }
  return id_event_pairs;
}

StatusOr<std::vector<InstrumentEvent>> Performer::RemoveInstrument(
    Id instrument_id) noexcept {
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

void Performer::SetSequenceBeginPosition(
    std::optional<double> sequence_begin_position) noexcept {
  sequence_begin_position_ = std::move(sequence_begin_position);
  sequence_position_offset_ =
      sequence_begin_position_ ? *sequence_begin_position_ : 0.0;
}

void Performer::SetSequenceEndPosition(
    std::optional<double> sequence_end_position) noexcept {
  sequence_end_position_ = std::move(sequence_end_position);
}

double Performer::ClampBeginPosition(double begin_position) noexcept {
  return sequence_begin_position_
             ? std::max(begin_position, *sequence_begin_position_)
             : begin_position;
}

double Performer::ClampEndPosition(double end_position) noexcept {
  return sequence_end_position_
             ? std::min(end_position, *sequence_end_position_)
             : end_position;
}

}  // namespace barely
