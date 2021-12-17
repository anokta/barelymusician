#include "barelymusician/engine/musician.h"

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

namespace barely {

namespace {

// Default playback tempo in BPM.
constexpr double kDefaultPlaybackTempo = 120.0;

// Converts seconds from minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Dummy playback update callback function that does nothing.
void NoopPlaybackUpdateCallback(double /*begin_position*/,
                                double /*end_position*/) noexcept {}

}  // namespace

Musician::Musician(int sample_rate) noexcept
    : instrument_manager_(sample_rate),
      playback_tempo_(kDefaultPlaybackTempo),
      playback_update_callback_(&NoopPlaybackUpdateCallback) {
  transport_.SetUpdateCallback(
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) noexcept {
        playback_update_callback_(begin_position, end_position);
        InstrumentIdEventPairMap id_event_pairs;
        for (auto& [performer_id, performer] : performers_) {
          id_event_pairs.merge(
              performer.Perform(begin_position, end_position, conductor_));
        }
        for (auto& [position, id_event_pair] : id_event_pairs) {
          auto& [instrument_id, event] = id_event_pair;
          instrument_manager_.ProcessEvent(
              instrument_id, get_timestamp_fn(position), std::move(event));
        }
      });
}

Id Musician::AddInstrument(InstrumentDefinition definition,
                           ParamDefinitionMap param_definitions) noexcept {
  const Id instrument_id = id_generator_.Next();
  instrument_manager_.Add(instrument_id, transport_.GetTimestamp(),
                          std::move(definition), std::move(param_definitions));
  return instrument_id;
}

Id Musician::AddPerformer() noexcept {
  const Id performer_id = id_generator_.Next();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

Status Musician::AddPerformerInstrument(Id performer_id,
                                        Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (instrument_manager_.IsValid(instrument_id)) {
      return performer->AddInstrument(instrument_id);
    }
  }
  return Status::kNotFound;
}

StatusOr<Id> Musician::AddPerformerNote(Id performer_id, double position,
                                        Note note) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Next();
    performer->GetMutableSequence()->AddNote(note_id, position,
                                             std::move(note));
    return note_id;
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerBeginPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    if (const auto begin_position = performer->GetSequenceBeginPosition()) {
      return *begin_position;
    }
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerEndPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    if (const auto end_position = performer->GetSequenceEndPosition()) {
      return *end_position;
    }
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerLoopBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerLoopLength(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopLength();
  }
  return Status::kNotFound;
}

double Musician::GetPlaybackPosition() const noexcept {
  return transport_.GetPosition();
}

double Musician::GetPlaybackTempo() const noexcept { return playback_tempo_; }

StatusOr<bool> Musician::IsPerformerEmpty(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Musician::IsPerformerLooping(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsLooping();
  }
  return Status::kNotFound;
}

bool Musician::IsPlaying() const noexcept { return transport_.IsPlaying(); }

void Musician::ProcessInstrument(Id instrument_id, double timestamp,
                                 float* output, int num_channels,
                                 int num_frames) noexcept {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
}

Status Musician::RemoveAllPerformerInstruments(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const double timestamp = transport_.GetTimestamp();
    for (auto& [instrument_id, event] : performer->RemoveAllInstruments()) {
      instrument_manager_.ProcessEvent(instrument_id, timestamp,
                                       std::move(event));
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::RemoveAllPerformerNotes(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                         double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes(begin_position,
                                                    end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::RemoveInstrument(Id instrument_id) noexcept {
  const auto status =
      instrument_manager_.Remove(instrument_id, transport_.GetTimestamp());
  if (IsOk(status)) {
    for (auto& [performer_id, performer] : performers_) {
      performer.RemoveInstrument(instrument_id);
    }
  }
  return status;
}

Status Musician::RemovePerformer(Id performer_id) noexcept {
  if (const auto performer_it = performers_.find(performer_id);
      performer_it != performers_.end()) {
    const double timestamp = transport_.GetTimestamp();
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

Status Musician::RemovePerformerInstrument(Id performer_id,
                                           Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const auto events_or = performer->RemoveInstrument(instrument_id);
    if (IsOk(events_or)) {
      const double timestamp = transport_.GetTimestamp();
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

Status Musician::RemovePerformerNote(Id performer_id, Id note_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetMutableSequence()->RemoveNote(note_id);
  }
  return Status::kNotFound;
}

void Musician::SetAllInstrumentNotesOff() noexcept {
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

Status Musician::SetAllInstrumentNotesOff(Id instrument_id) noexcept {
  return instrument_manager_.SetAllNotesOff(instrument_id,
                                            transport_.GetTimestamp());
}

void Musician::SetAllInstrumentParamsToDefault() noexcept {
  instrument_manager_.SetAllParamsToDefault(transport_.GetTimestamp());
}

Status Musician::SetAllInstrumentParamsToDefault(Id instrument_id) noexcept {
  return instrument_manager_.SetAllParamsToDefault(instrument_id,
                                                   transport_.GetTimestamp());
}

Status Musician::SetCustomInstrumentData(Id instrument_id,
                                         std::any custom_data) noexcept {
  return instrument_manager_.SetCustomData(
      instrument_id, transport_.GetTimestamp(), std::move(custom_data));
}

void Musician::SetConductor(ConductorDefinition definition,
                            ParamDefinitionMap param_definitions) noexcept {
  conductor_ = Conductor{std::move(definition), std::move(param_definitions)};
}

Status Musician::SetInstrumentNoteOff(Id instrument_id,
                                      float note_pitch) noexcept {
  return instrument_manager_.SetNoteOff(instrument_id,
                                        transport_.GetTimestamp(), note_pitch);
}

void Musician::SetInstrumentNoteOffCallback(
    InstrumentNoteOffCallback instrument_note_off_callback) noexcept {
  instrument_manager_.SetNoteOffCallback(
      std::move(instrument_note_off_callback));
}

Status Musician::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                     float note_intensity) noexcept {
  return instrument_manager_.SetNoteOn(instrument_id, transport_.GetTimestamp(),
                                       note_pitch, note_intensity);
}

Status Musician::SetInstrumentParam(Id instrument_id, int param_id,
                                    float param_value) noexcept {
  return instrument_manager_.SetParam(instrument_id, transport_.GetTimestamp(),
                                      param_id, param_value);
}

Status Musician::SetInstrumentParamToDefault(Id instrument_id,
                                             int param_id) noexcept {
  return instrument_manager_.SetParamToDefault(
      instrument_id, transport_.GetTimestamp(), param_id);
}

void Musician::SetInstrumentNoteOnCallback(
    InstrumentNoteOnCallback instrument_note_on_callback) noexcept {
  instrument_manager_.SetNoteOnCallback(std::move(instrument_note_on_callback));
}

Status Musician::SetPerformerBeginOffset(Id performer_id,
                                         double begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerBeginPosition(
    Id performer_id, std::optional<double> begin_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceBeginPosition(std::move(begin_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerEndPosition(
    Id performer_id, std::optional<double> end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceEndPosition(std::move(end_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoop(Id performer_id, bool loop) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoop(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoopBeginOffset(
    Id performer_id, double loop_begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoopLength(Id performer_id,
                                        double loop_length) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Musician::SetPlaybackBeatCallback(
    PlaybackBeatCallback playback_beat_callback) noexcept {
  transport_.SetBeatCallback(std::move(playback_beat_callback));
}

void Musician::SetPlaybackPosition(double position) noexcept {
  transport_.SetPosition(position);
}

void Musician::SetPlaybackTempo(double tempo) noexcept {
  playback_tempo_ = std::max(tempo, 0.0);
}

void Musician::SetPlaybackUpdateCallback(
    PlaybackUpdateCallback playback_update_callback) noexcept {
  playback_update_callback_ = playback_update_callback
                                  ? std::move(playback_update_callback)
                                  : &NoopPlaybackUpdateCallback;
}

void Musician::SetSampleRate(int sample_rate) noexcept {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  instrument_manager_.SetSampleRate(transport_.GetTimestamp(),
                                    std::max(sample_rate, 0));
}

void Musician::StartPlayback() noexcept { transport_.Start(); }

void Musician::StopPlayback() noexcept {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  transport_.Stop();
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

void Musician::Update(double timestamp) noexcept {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barely
