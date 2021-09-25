#include "barelymusician/engine/sequencer.h"

#include <cmath>
#include <iterator>
#include <utility>
#include <variant>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Sequencer::Sequencer(InstrumentManager* manager) : manager_(manager) {
  clock_.SetUpdateCallback([&](double begin_position, double end_position) {
    // Trigger messages.
    if (begin_position < end_position) {
      for (auto& [id, track] : tracks_) {
        auto begin = track.events.lower_bound(begin_position);
        auto end = track.events.lower_bound(end_position);
        const auto instrument_id = id;
        for (auto it = begin; it != end; ++it) {
          std::visit(
              InstrumentEventVisitor{
                  [&](SetNoteOffEvent& set_note_off_event) {
                    manager_->SetNoteOff(
                        instrument_id, clock_.GetTimestampAtPosition(it->first),
                        set_note_off_event.pitch);
                  },
                  [&](SetNoteOnEvent& set_note_on_event) {
                    manager_->SetNoteOn(
                        instrument_id, clock_.GetTimestampAtPosition(it->first),
                        set_note_on_event.pitch, set_note_on_event.intensity);
                  },
                  [](auto&) {}},
              it->second);
        }
      }
    } else {
      // Trigger messages backwards.
      for (auto& [id, track] : tracks_) {
        auto begin =
            std::reverse_iterator(track.events.upper_bound(begin_position));
        auto end =
            std::reverse_iterator(track.events.upper_bound(end_position));
        const auto instrument_id = id;
        for (auto it = begin; it != end; ++it) {
          std::visit(
              InstrumentEventVisitor{
                  [&](SetNoteOffEvent& set_note_off_event) {
                    // TODO: intensity missing.
                    manager_->SetNoteOn(
                        instrument_id, clock_.GetTimestampAtPosition(it->first),
                        set_note_off_event.pitch, 0.5);
                  },
                  [&](SetNoteOnEvent& set_note_on_event) {
                    manager_->SetNoteOff(
                        instrument_id, clock_.GetTimestampAtPosition(it->first),
                        set_note_on_event.pitch);
                  },
                  [](auto&) {}},
              it->second);
        }
      }
    }
  });
}

double Sequencer::GetPlaybackPosition() const { return clock_.GetPosition(); }

double Sequencer::GetPlaybackTempo() const { return tempo_; }

bool Sequencer::IsPlaying() const { return is_playing_; }

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
  track_it->second.events.emplace(note_begin_position,
                                  SetNoteOnEvent{note_pitch, note_intensity});
  track_it->second.events.emplace(note_end_position,
                                  SetNoteOffEvent{note_pitch});
}

void Sequencer::SetBeatCallback(BeatCallback beat_callback) {
  if (beat_callback) {
    clock_.SetBeatCallback([beat_callback](double beat, double /*timestamp*/) {
      beat_callback(beat);
    });
  } else {
    clock_.SetBeatCallback(nullptr);
  }
}

void Sequencer::SetPlaybackPosition(double position) {
  StopAllNotes();
  clock_.SetPosition(position);
}

void Sequencer::SetPlaybackTempo(double tempo) {
  tempo_ = tempo;
  if (is_playing_) {
    clock_.SetTempo(tempo_);
  }
}

void Sequencer::StartPlayback() {
  clock_.SetTempo(tempo_);
  is_playing_ = true;
}

void Sequencer::StopPlayback() {
  StopAllNotes();
  clock_.SetTempo(0.0);
  is_playing_ = false;
}

void Sequencer::Update(double timestamp) { clock_.UpdatePosition(timestamp); }

void Sequencer::StopAllNotes() {
  const double timestamp = clock_.GetTimestamp();
  for (auto& [id, track] : tracks_) {
    manager_->SetAllNotesOff(id, timestamp);
    track.events.clear();
  }
}

}  // namespace barelyapi
