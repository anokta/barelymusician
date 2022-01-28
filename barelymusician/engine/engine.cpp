#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

namespace {

// Default playback tempo in bpm.
constexpr double kDefaultPlaybackTempo = 120.0;

// Converts seconds from minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

/// Instrument id-event pair.
using InstrumentIdEventPair = std::pair<Id, InstrumentEvent>;

/// Instrument id-event pair by position map type.
using InstrumentIdEventPairMap = std::multimap<double, InstrumentIdEventPair>;

}  // namespace

Engine::Engine() noexcept : playback_tempo_(kDefaultPlaybackTempo) {
  transport_.SetUpdateCallback([this](double begin_position,
                                      double end_position) noexcept {
    InstrumentIdEventPairMap id_event_pairs;
    for (auto& [performer_id, performer] : performers_) {
      const auto instrument_id = performer.instrument_id;
      // Perform active note events.
      for (auto it = performer.active_notes.begin();
           it != performer.active_notes.end();) {
        const auto& [note_begin_position, active_note] = *it;
        double note_end_position = active_note.end_position;
        if (note_end_position < end_position) {
          note_end_position = std::max(begin_position, note_end_position);
        } else if (begin_position < note_begin_position) {
          note_end_position = begin_position;
        } else {
          ++it;
          continue;
        }
        // Perform note off event.
        id_event_pairs.emplace(
            note_end_position,
            InstrumentIdEventPair{instrument_id,
                                  StopNoteEvent{active_note.pitch}});
        it = performer.active_notes.erase(it);
      }

      // Perform sequence events.
      performer.sequence.Process(
          begin_position, end_position,
          [&](double position, const Note& note) noexcept {
            // Get pitch.
            const auto pitch_or = conductor_.TransformNotePitch(note.pitch);
            if (!IsOk(pitch_or)) {
              return;
            }
            const float pitch = GetStatusOrValue(pitch_or);
            // Get intensity.
            const auto intensity_or =
                conductor_.TransformNoteIntensity(note.intensity);
            if (!IsOk(intensity_or)) {
              return;
            }
            const float intensity = GetStatusOrValue(intensity_or);
            // Get duration.
            const auto duration_or =
                conductor_.TransformNoteDuration(note.duration);
            if (!IsOk(duration_or)) {
              return;
            }
            const double note_end_position = std::min(
                position + std::max(GetStatusOrValue(duration_or), 0.0),
                performer.sequence.GetEndPosition());

            // Perform note on event.
            id_event_pairs.emplace(
                position, InstrumentIdEventPair{
                              instrument_id, StartNoteEvent{pitch, intensity}});
            // Perform note off event.
            if (note_end_position < end_position) {
              id_event_pairs.emplace(
                  note_end_position,
                  InstrumentIdEventPair{instrument_id, StopNoteEvent{pitch}});
            } else {
              performer.active_notes.emplace(
                  position, ActiveNote{note_end_position, pitch});
            }
          });
    }
    for (auto& [position, id_event_pair] : id_event_pairs) {
      auto& [instrument_id, event] = id_event_pair;
      instrument_manager_.ProcessEvent(
          instrument_id, transport_.GetTimestamp(position), std::move(event));
    }
  });
}

Id Engine::AddInstrument(InstrumentDefinition definition,
                         int sample_rate) noexcept {
  const Id instrument_id = id_generator_.Next();
  instrument_manager_.Create(instrument_id, std::move(definition), sample_rate);
  return instrument_id;
}

Id Engine::AddPerformer() noexcept {
  const Id performer_id = id_generator_.Next();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

StatusOr<Id> Engine::AddPerformerNote(Id performer_id, double position,
                                      Note note) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Next();
    performer->sequence.AddNote(note_id, position, std::move(note));
    return note_id;
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetBeginPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerEndPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetEndPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopLength(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetLoopLength();
  }
  return Status::kNotFound;
}

double Engine::GetPlaybackPosition() const noexcept {
  return transport_.GetPosition();
}

double Engine::GetPlaybackTempo() const noexcept { return playback_tempo_; }

StatusOr<bool> Engine::IsPerformerEmpty(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsPerformerLooping(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.IsLooping();
  }
  return Status::kNotFound;
}

bool Engine::IsPlaying() const noexcept { return transport_.IsPlaying(); }

void Engine::ProcessInstrument(Id instrument_id, double timestamp,
                               float* output, int num_channels,
                               int num_frames) noexcept {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
}

Status Engine::RemoveAllPerformerNotes(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveInstrument(Id instrument_id) noexcept {
  const auto status = instrument_manager_.SetAllNotesOff(
      instrument_id, transport_.GetTimestamp());
  if (IsOk(status)) {
    instrument_manager_.Destroy(instrument_id);
    for (auto& [performer_id, performer] : performers_) {
      if (performer.instrument_id == instrument_id) {
        performer.instrument_id = kInvalidId;
      }
    }
  }
  return status;
}

Status Engine::RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                       double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.RemoveAllNotes(begin_position, end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformer(Id performer_id) noexcept {
  if (const auto performer_it = performers_.find(performer_id);
      performer_it != performers_.end()) {
    const double timestamp = transport_.GetTimestamp();
    const auto instrument_id = performer_it->second.instrument_id;
    if (instrument_id != kInvalidId) {
      for (auto& [position, active_note] : performer_it->second.active_notes) {
        instrument_manager_.ProcessEvent(instrument_id, timestamp,
                                         StopNoteEvent{active_note.pitch});
      }
    }
    performers_.erase(performer_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformerNote(Id performer_id, Id note_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.RemoveNote(note_id);
  }
  return Status::kNotFound;
}

Status Engine::SetAllInstrumentNotesOff(Id instrument_id) noexcept {
  return instrument_manager_.SetAllNotesOff(instrument_id,
                                            transport_.GetTimestamp());
}

Status Engine::SetAllInstrumentParamsToDefault(Id instrument_id) noexcept {
  return instrument_manager_.SetAllParamsToDefault(instrument_id,
                                                   transport_.GetTimestamp());
}

Status Engine::SetInstrumentData(Id instrument_id, void* data) noexcept {
  return instrument_manager_.SetData(instrument_id, transport_.GetTimestamp(),
                                     data);
}

void Engine::SetConductor(ConductorDefinition definition) noexcept {
  conductor_ = Conductor{std::move(definition)};
}

Status Engine::SetInstrumentGain(Id instrument_id, float gain) noexcept {
  return instrument_manager_.SetGain(instrument_id, transport_.GetTimestamp(),
                                     gain);
}

Status Engine::SetInstrumentMuted(Id instrument_id, bool is_muted) noexcept {
  return instrument_manager_.SetMuted(instrument_id, transport_.GetTimestamp(),
                                      is_muted);
}

Status Engine::SetInstrumentNoteOff(Id instrument_id,
                                    float note_pitch) noexcept {
  return instrument_manager_.SetNoteOff(instrument_id,
                                        transport_.GetTimestamp(), note_pitch);
}

void Engine::SetInstrumentNoteOffCallback(
    Id instrument_id, NoteOffCallback note_off_callback) noexcept {
  instrument_manager_.SetNoteOffCallback(instrument_id,
                                         std::move(note_off_callback));
}

Status Engine::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                   float note_intensity) noexcept {
  return instrument_manager_.SetNoteOn(instrument_id, transport_.GetTimestamp(),
                                       note_pitch, note_intensity);
}

Status Engine::SetInstrumentParam(Id instrument_id, int param_index,
                                  float param_value) noexcept {
  return instrument_manager_.SetParam(instrument_id, transport_.GetTimestamp(),
                                      param_index, param_value);
}

Status Engine::SetInstrumentParamToDefault(Id instrument_id,
                                           int param_index) noexcept {
  return instrument_manager_.SetParamToDefault(
      instrument_id, transport_.GetTimestamp(), param_index);
}

void Engine::SetInstrumentNoteOnCallback(
    Id instrument_id, NoteOnCallback note_on_callback) noexcept {
  instrument_manager_.SetNoteOnCallback(instrument_id,
                                        std::move(note_on_callback));
}

Status Engine::SetPerformerBeginOffset(Id performer_id,
                                       double begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerBeginPosition(Id performer_id,
                                         double begin_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetBeginPosition(begin_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerEndPosition(Id performer_id,
                                       double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetEndPosition(end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoop(Id performer_id, bool loop) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoop(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerInstrument(Id performer_id,
                                      Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (performer->instrument_id != instrument_id) {
      for (auto& [position, active_note] : performer->active_notes) {
        instrument_manager_.ProcessEvent(instrument_id,
                                         transport_.GetTimestamp(),
                                         StopNoteEvent{active_note.pitch});
      }
      performer->instrument_id = instrument_id;
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopBeginOffset(Id performer_id,
                                           double loop_begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopLength(Id performer_id,
                                      double loop_length) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetPlaybackBeatCallback(BeatCallback beat_callback) noexcept {
  transport_.SetBeatCallback(std::move(beat_callback));
}

void Engine::SetPlaybackPosition(double position) noexcept {
  transport_.SetPosition(position);
}

void Engine::SetPlaybackTempo(double tempo) noexcept {
  playback_tempo_ = std::max(tempo, 0.0);
}

void Engine::StartPlayback() noexcept { transport_.Start(); }

void Engine::StopPlayback() noexcept {
  for (auto& [performer_id, performer] : performers_) {
    performer.active_notes.clear();
  }
  transport_.Stop();
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

void Engine::Update(double timestamp) noexcept {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barelyapi
