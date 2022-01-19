#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <limits>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Performer::Performer() noexcept
    : sequence_begin_position_(0.0),
      sequence_end_position_(std::numeric_limits<double>::max()) {}

void Performer::ClearAllActiveNotes() noexcept { active_notes_.clear(); }

Id Performer::GetInstrument() const noexcept { return instrument_id_; }

double Performer::GetSequenceBeginPosition() const noexcept {
  return sequence_begin_position_;
}

double Performer::GetSequenceEndPosition() const noexcept {
  return sequence_end_position_;
}

Sequence* Performer::GetMutableSequence() noexcept { return &sequence_; }

const Sequence& Performer::GetSequence() const noexcept { return sequence_; }

InstrumentIdEventPairMap Performer::Perform(double begin_position,
                                            double end_position,
                                            Conductor& conductor) noexcept {
  InstrumentIdEventPairMap id_event_pairs;

  // Perform active note events.
  for (auto it = active_notes_.begin(); it != active_notes_.end();) {
    const auto& [note_begin_position, active_note] = *it;
    double note_end_position = begin_position;
    const auto duration_or =
        conductor.TransformNoteDuration(active_note.duration);
    if (IsOk(duration_or)) {
      note_end_position =
          std::min(note_begin_position + GetStatusOrValue(duration_or),
                   sequence_end_position_);
      if (note_end_position < end_position) {
        note_end_position = std::max(begin_position, note_end_position);
      } else if (begin_position <
                 std::max(note_begin_position, begin_position)) {
        note_end_position = begin_position;
      } else {
        ++it;
        continue;
      }
    }
    // Perform note off event.
    id_event_pairs.emplace(
        note_end_position,
        InstrumentIdEventPair{instrument_id_,
                              StopNoteEvent{active_note.pitch}});
    it = active_notes_.erase(it);
  }

  // Perform sequence events.
  begin_position = std::max(begin_position, sequence_begin_position_);
  end_position = std::min(end_position, sequence_end_position_);
  if (!sequence_.IsEmpty() && begin_position < end_position) {
    sequence_.Process(
        begin_position, end_position, sequence_begin_position_,
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

          // Perform note on event.
          id_event_pairs.emplace(
              position, InstrumentIdEventPair{
                            instrument_id_, StartNoteEvent{pitch, intensity}});
          // Perform note off event.
          if (note_end_position < end_position) {
            id_event_pairs.emplace(
                note_end_position,
                InstrumentIdEventPair{instrument_id_, StopNoteEvent{pitch}});
          } else {
            active_notes_.emplace(position, ActiveNote{note.duration, pitch});
          }
        });
  }

  return id_event_pairs;
}

std::vector<InstrumentEvent> Performer::SetInstrument(
    Id instrument_id) noexcept {
  std::vector<InstrumentEvent> events;
  if (instrument_id_ != instrument_id) {
    instrument_id_ = instrument_id;
    for (const auto& [position, active_note] :
         std::exchange(active_notes_, {})) {
      events.push_back(StopNoteEvent{active_note.pitch});
    }
  }
  return events;
}

void Performer::SetSequenceBeginPosition(
    double sequence_begin_position) noexcept {
  sequence_begin_position_ = sequence_begin_position;
}

void Performer::SetSequenceEndPosition(double sequence_end_position) noexcept {
  sequence_end_position_ = sequence_end_position;
}

}  // namespace barelyapi
