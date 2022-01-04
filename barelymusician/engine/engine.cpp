#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <any>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/param_definition.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

namespace {

// Default playback tempo in BPM.
constexpr double kDefaultPlaybackTempo = 120.0;

// Converts seconds from minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Dummy playback update callback function that does nothing.
void NoopPlaybackUpdateCallback(double /*begin_position*/,
                                double /*end_position*/) noexcept {}

}  // namespace

Engine::Engine(int sample_rate) noexcept
    : instrument_manager_(sample_rate),
      playback_tempo_(kDefaultPlaybackTempo),
      playback_update_callback_(&NoopPlaybackUpdateCallback) {
  transport_.SetUpdateCallback([&](double begin_position,
                                   double end_position) noexcept {
    playback_update_callback_(begin_position, end_position);
    InstrumentIdEventPairMap id_event_pairs;
    for (auto& [performer_id, performer] : performers_) {
      id_event_pairs.merge(
          performer.Perform(begin_position, end_position, conductor_));
    }
    for (auto& [position, id_event_pair] : id_event_pairs) {
      auto& [instrument_id, event] = id_event_pair;
      instrument_manager_.ProcessEvent(instrument_id,
                                       transport_.GetLastTimestamp(position),
                                       std::move(event));
    }
  });
}

Id Engine::AddInstrument(InstrumentDefinition definition) noexcept {
  const Id instrument_id = id_generator_.Next();
  instrument_manager_.Add(instrument_id, transport_.GetLastTimestamp(),
                          std::move(definition));
  return instrument_id;
}

Id Engine::AddPerformer() noexcept {
  const Id performer_id = id_generator_.Next();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

Status Engine::AddPerformerInstrument(Id performer_id,
                                      Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (instrument_manager_.IsValid(instrument_id)) {
      return performer->AddInstrument(instrument_id);
    }
  }
  return Status::kNotFound;
}

StatusOr<Id> Engine::AddPerformerNote(Id performer_id, double position,
                                      Note note) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Next();
    performer->GetMutableSequence()->AddNote(note_id, position,
                                             std::move(note));
    return note_id;
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    if (const auto begin_position = performer->GetSequenceBeginPosition()) {
      return *begin_position;
    }
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerEndPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    if (const auto end_position = performer->GetSequenceEndPosition()) {
      return *end_position;
    }
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopLength(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopLength();
  }
  return Status::kNotFound;
}

double Engine::GetPlaybackPosition() const noexcept {
  return transport_.GetPosition();
}

double Engine::GetPlaybackTempo() const noexcept { return playback_tempo_; }

StatusOr<bool> Engine::IsPerformerEmpty(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsPerformerLooping(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsLooping();
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

Status Engine::RemoveAllPerformerInstruments(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const double timestamp = transport_.GetLastTimestamp();
    for (auto& [instrument_id, event] : performer->RemoveAllInstruments()) {
      instrument_manager_.ProcessEvent(instrument_id, timestamp,
                                       std::move(event));
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllPerformerNotes(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                       double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes(begin_position,
                                                    end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveInstrument(Id instrument_id) noexcept {
  const auto status =
      instrument_manager_.Remove(instrument_id, transport_.GetLastTimestamp());
  if (IsOk(status)) {
    for (auto& [performer_id, performer] : performers_) {
      performer.RemoveInstrument(instrument_id);
    }
  }
  return status;
}

Status Engine::RemovePerformer(Id performer_id) noexcept {
  if (const auto performer_it = performers_.find(performer_id);
      performer_it != performers_.end()) {
    const double timestamp = transport_.GetLastTimestamp();
    for (auto& [instrument_id, event] :
         performer_it->second.RemoveAllInstruments()) {
      instrument_manager_.ProcessEvent(instrument_id, timestamp,
                                       std::move(event));
    }
    performers_.erase(performer_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformerInstrument(Id performer_id,
                                         Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const auto events_or = performer->RemoveInstrument(instrument_id);
    if (IsOk(events_or)) {
      const double timestamp = transport_.GetLastTimestamp();
      for (auto& event : GetStatusOrValue(events_or)) {
        instrument_manager_.ProcessEvent(instrument_id, timestamp,
                                         std::move(event));
      }
      return Status::kOk;
    }
    return GetStatusOrStatus(events_or);
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformerNote(Id performer_id, Id note_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetMutableSequence()->RemoveNote(note_id);
  }
  return Status::kNotFound;
}

Status Engine::SetAllInstrumentNotesOff(Id instrument_id) noexcept {
  return instrument_manager_.SetAllNotesOff(instrument_id,
                                            transport_.GetLastTimestamp());
}

Status Engine::SetAllInstrumentParamsToDefault(Id instrument_id) noexcept {
  return instrument_manager_.SetAllParamsToDefault(
      instrument_id, transport_.GetLastTimestamp());
}

Status Engine::SetCustomInstrumentData(Id instrument_id,
                                       std::any custom_data) noexcept {
  return instrument_manager_.SetCustomData(
      instrument_id, transport_.GetLastTimestamp(), std::move(custom_data));
}

void Engine::SetConductor(ConductorDefinition definition) noexcept {
  conductor_ = Conductor{std::move(definition)};
}

Status Engine::SetInstrumentGain(Id instrument_id, float gain) noexcept {
  return instrument_manager_.SetGain(instrument_id,
                                     transport_.GetLastTimestamp(), gain);
}

Status Engine::SetInstrumentMuted(Id instrument_id, bool is_muted) noexcept {
  return instrument_manager_.SetMuted(instrument_id,
                                      transport_.GetLastTimestamp(), is_muted);
}

Status Engine::SetInstrumentNoteOff(Id instrument_id,
                                    float note_pitch) noexcept {
  return instrument_manager_.SetNoteOff(
      instrument_id, transport_.GetLastTimestamp(), note_pitch);
}

void Engine::SetInstrumentNoteOffCallback(
    InstrumentNoteOffCallback instrument_note_off_callback) noexcept {
  instrument_manager_.SetNoteOffCallback(
      std::move(instrument_note_off_callback));
}

Status Engine::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                   float note_intensity) noexcept {
  return instrument_manager_.SetNoteOn(
      instrument_id, transport_.GetLastTimestamp(), note_pitch, note_intensity);
}

Status Engine::SetInstrumentParam(Id instrument_id, int param_id,
                                  float param_value) noexcept {
  return instrument_manager_.SetParam(
      instrument_id, transport_.GetLastTimestamp(), param_id, param_value);
}

Status Engine::SetInstrumentParamToDefault(Id instrument_id,
                                           int param_id) noexcept {
  return instrument_manager_.SetParamToDefault(
      instrument_id, transport_.GetLastTimestamp(), param_id);
}

void Engine::SetInstrumentNoteOnCallback(
    InstrumentNoteOnCallback instrument_note_on_callback) noexcept {
  instrument_manager_.SetNoteOnCallback(std::move(instrument_note_on_callback));
}

Status Engine::SetPerformerBeginOffset(Id performer_id,
                                       double begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerBeginPosition(
    Id performer_id, std::optional<double> begin_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceBeginPosition(std::move(begin_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerEndPosition(
    Id performer_id, std::optional<double> end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceEndPosition(std::move(end_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoop(Id performer_id, bool loop) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoop(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopBeginOffset(Id performer_id,
                                           double loop_begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopLength(Id performer_id,
                                      double loop_length) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetPlaybackBeatCallback(
    PlaybackBeatCallback playback_beat_callback) noexcept {
  transport_.SetBeatCallback(std::move(playback_beat_callback));
}

void Engine::SetPlaybackPosition(double position) noexcept {
  transport_.SetPosition(position);
}

void Engine::SetPlaybackTempo(double tempo) noexcept {
  playback_tempo_ = std::max(tempo, 0.0);
}

void Engine::SetPlaybackUpdateCallback(
    PlaybackUpdateCallback playback_update_callback) noexcept {
  playback_update_callback_ = playback_update_callback
                                  ? std::move(playback_update_callback)
                                  : &NoopPlaybackUpdateCallback;
}

void Engine::SetSampleRate(int sample_rate) noexcept {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  instrument_manager_.SetSampleRate(transport_.GetLastTimestamp(),
                                    std::max(sample_rate, 0));
}

void Engine::StartPlayback() noexcept { transport_.Start(); }

void Engine::StopPlayback() noexcept {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  transport_.Stop();
  instrument_manager_.SetAllNotesOff(transport_.GetLastTimestamp());
}

void Engine::Update(double timestamp) noexcept {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barelyapi
