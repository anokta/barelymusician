#include "barelymusician/engine/sequencer.h"

#include <cmath>
#include <utility>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Sequencer::Sequencer(InstrumentManager* manager)
    : manager_(manager), beat_callback_(nullptr) {}

double Sequencer::GetPlaybackPosition() const { return clock_.GetPosition(); }

double Sequencer::GetPlaybackTempo() const { return clock_.GetTempo(); }

bool Sequencer::IsPlaying() const { return clock_.IsPlaying(); }

void Sequencer::RemoveAllScheduledInstrumentNotes() { tracks_.clear(); }

void Sequencer::RemoveAllScheduledInstrumentNotes(Id instrument_id) {
  tracks_.erase(instrument_id);
}

void Sequencer::ScheduleInstrumentNote(Id instrument_id,
                                       double note_begin_position,
                                       double note_end_position,
                                       float note_pitch, float note_intensity) {
  auto track_it = tracks_.find(instrument_id);
  if (track_it == tracks_.end()) {
    track_it = tracks_.emplace(instrument_id, Track{}).first;
  }
  track_it->second.score.emplace(
      note_begin_position,
      Note{note_begin_position, note_end_position, note_pitch, note_intensity});
}

void Sequencer::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Sequencer::SetPlaybackPosition(double position) {
  StopAllNotes();
  clock_.SetPosition(position);
}

void Sequencer::SetPlaybackTempo(double tempo) { clock_.SetTempo(tempo); }

void Sequencer::StartPlayback() { clock_.Start(); }

void Sequencer::StopPlayback() {
  StopAllNotes();
  clock_.Stop();
}

void Sequencer::Update(double timestamp) {
  const double begin_position = clock_.GetPosition();
  clock_.Update(timestamp);
  const double end_position = clock_.GetPosition();
  if (begin_position < end_position) {
    // Trigger beats.
    if (beat_callback_) {
      for (double beat = std::ceil(begin_position); beat < end_position;
           ++beat) {
        beat_callback_(static_cast<int>(beat));
      }
    }
    // Trigger messages.
    for (auto& [id, track] : tracks_) {
      InstrumentControllerEvents events;

      // Handle note offs.
      auto begin = track.active_notes.lower_bound(begin_position);
      auto end = track.active_notes.lower_bound(end_position);
      for (auto it = begin; it != end; ++it) {
        events.emplace(clock_.GetTimestampAtPosition(it->first),
                       SetNoteOff{it->second.pitch});
      }
      track.active_notes.erase(begin, end);

      // Handle score.
      begin = track.score.lower_bound(begin_position);
      end = track.score.lower_bound(end_position);
      for (auto it = begin; it != end; ++it) {
        const auto& note = it->second;
        events.emplace(clock_.GetTimestampAtPosition(it->first),
                       SetNoteOn{note.pitch, note.intensity});

        if (note.end_position < end_position) {
          events.emplace(clock_.GetTimestampAtPosition(note.end_position),
                         SetNoteOff{note.pitch});
        } else {
          track.active_notes.emplace(note.end_position, note);
        }
      }

      manager_->SetEvents(id, std::move(events));
    }
  }
}

void Sequencer::StopAllNotes() {
  const double timestamp = clock_.GetTimestamp();
  for (auto& [id, track] : tracks_) {
    InstrumentControllerEvents events;
    for (const auto& [position, note] : track.active_notes) {
      events.emplace(timestamp, SetNoteOff{note.pitch});
    }
    manager_->SetEvents(id, std::move(events));
    track.active_notes.clear();
  }
}

}  // namespace barelyapi
