#include "barelymusician/engine/sequencer.h"

#include <cmath>
#include <iterator>
#include <utility>
#include <variant>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

Sequencer::Sequencer(InstrumentManager* manager) : manager_(manager) {
  transport_.SetUpdateCallback(
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn get_timestamp_fn) {
        // Trigger messages.
        for (auto& [id, track] : tracks_) {
          const auto begin = track.events.lower_bound(begin_position);
          const auto end = track.events.lower_bound(end_position);
          const auto instrument_id = id;
          for (auto it = begin; it != end; ++it) {
            std::visit(InstrumentEventVisitor{
                           [&](SetNoteOffEvent& set_note_off_event) {
                             manager_->SetNoteOff(instrument_id,
                                                  get_timestamp_fn(it->first),
                                                  set_note_off_event.pitch);
                           },
                           [&](SetNoteOnEvent& set_note_on_event) {
                             manager_->SetNoteOn(instrument_id,
                                                 get_timestamp_fn(it->first),
                                                 set_note_on_event.pitch,
                                                 set_note_on_event.intensity);
                           },
                           [](auto&) {}},
                       it->second);
          }
        }
      });
}

double Sequencer::GetPlaybackPosition() const {
  return transport_.GetPosition();
}

double Sequencer::GetPlaybackTempo() const { return transport_.GetTempo(); }

bool Sequencer::IsPlaying() const { return transport_.IsPlaying(); }

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
  transport_.SetBeatCallback(std::move(beat_callback));
}

void Sequencer::SetPlaybackPosition(double position) {
  StopAllNotes();
  transport_.SetPosition(position);
}

void Sequencer::SetPlaybackTempo(double tempo) { transport_.SetTempo(tempo); }

void Sequencer::StartPlayback() { transport_.Start(); }

void Sequencer::StopPlayback() {
  StopAllNotes();
  transport_.Stop();
}

void Sequencer::Update(double timestamp) { transport_.Update(timestamp); }

void Sequencer::StopAllNotes() {
  const double timestamp = transport_.GetTimestamp();
  for (auto& [id, track] : tracks_) {
    manager_->SetAllNotesOff(id, timestamp);
    track.events.clear();
  }
}

}  // namespace barelyapi
