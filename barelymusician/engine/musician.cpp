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

// Dummy instrument note off callback function that does nothing.
void NoopInstrumentNoteOffCallback(Id /*instrument_id*/, float /*note_pitch*/) {
}

// Dummy instrument note on callback function that does nothing.
void NoopInstrumentNoteOnCallback(Id /*instrument_id*/, float /*note_pitch*/,
                                  float /*note_intensity*/) {}

// Dummy playback update callback function that does nothing.
void NoopPlaybackUpdateCallback(double /*begin_position*/,
                                double /*end_position*/) {}

}  // namespace

Musician::Musician(int sample_rate)
    : instrument_manager_(sample_rate),
      instrument_note_off_callback_(&NoopInstrumentNoteOffCallback),
      instrument_note_on_callback_(&NoopInstrumentNoteOnCallback),
      playback_tempo_(kDefaultPlaybackTempo),
      playback_update_callback_(&NoopPlaybackUpdateCallback) {
  instrument_manager_.SetNoteOffCallback(
      [&](Id instrument_id, double /*timestamp*/, float note_pitch) {
        instrument_note_off_callback_(instrument_id, note_pitch);
      });
  instrument_manager_.SetNoteOnCallback(
      [&](Id instrument_id, double /*timestamp*/, float note_pitch,
          float note_intensity) {
        instrument_note_on_callback_(instrument_id, note_pitch, note_intensity);
      });
  transport_.SetUpdateCallback(
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        playback_update_callback_(begin_position, end_position);
        InstrumentIdEventPairs id_event_pairs;
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
                           ParamDefinitions param_definitions) {
  const Id instrument_id = id_generator_.Next();
  instrument_manager_.Add(instrument_id, transport_.GetTimestamp(),
                          std::move(definition), std::move(param_definitions));
  return instrument_id;
}

Id Musician::AddPerformer() {
  const Id performer_id = id_generator_.Next();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

Status Musician::AddPerformerInstrument(Id performer_id, Id instrument_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (instrument_manager_.IsValid(instrument_id)) {
      return performer->AddInstrument(instrument_id);
    }
  }
  return Status::kNotFound;
}

StatusOr<Id> Musician::AddPerformerNote(Id performer_id, double position,
                                        Note note) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Next();
    performer->GetMutableSequence()->AddNote(note_id, position,
                                             std::move(note));
    return note_id;
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerBeginOffset(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<std::optional<double>> Musician::GetPerformerBeginPosition(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequenceBeginPosition();
  }
  return Status::kNotFound;
}

StatusOr<std::optional<double>> Musician::GetPerformerEndPosition(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequenceEndPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerLoopBeginOffset(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Musician::GetPerformerLoopLength(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopLength();
  }
  return Status::kNotFound;
}

double Musician::GetPlaybackPosition() const {
  return transport_.GetPosition();
}

double Musician::GetPlaybackTempo() const { return playback_tempo_; }

StatusOr<bool> Musician::IsPerformerEmpty(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Musician::IsPerformerLooping(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsLooping();
  }
  return Status::kNotFound;
}

bool Musician::IsPlaying() const { return transport_.IsPlaying(); }

void Musician::ProcessInstrument(Id instrument_id, double timestamp,
                                 float* output, int num_channels,
                                 int num_frames) {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
}

Status Musician::RemoveAllPerformerInstruments(Id performer_id) {
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

Status Musician::RemoveAllPerformerNotes(Id performer_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                         double end_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes(begin_position,
                                                    end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::RemoveInstrument(Id instrument_id) {
  const auto status =
      instrument_manager_.Remove(instrument_id, transport_.GetTimestamp());
  if (IsOk(status)) {
    for (auto& [performer_id, performer] : performers_) {
      performer.RemoveInstrument(instrument_id);
    }
  }
  return status;
}

Status Musician::RemovePerformer(Id performer_id) {
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

Status Musician::RemovePerformerInstrument(Id performer_id, Id instrument_id) {
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

Status Musician::RemovePerformerNote(Id performer_id, Id note_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetMutableSequence()->RemoveNote(note_id);
  }
  return Status::kNotFound;
}

void Musician::SetAllInstrumentNotesOff() {
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

Status Musician::SetAllInstrumentNotesOff(Id instrument_id) {
  return instrument_manager_.SetAllNotesOff(instrument_id,
                                            transport_.GetTimestamp());
}

void Musician::SetAllInstrumentParamsToDefault() {
  instrument_manager_.SetAllParamsToDefault(transport_.GetTimestamp());
}

Status Musician::SetAllInstrumentParamsToDefault(Id instrument_id) {
  return instrument_manager_.SetAllParamsToDefault(instrument_id,
                                                   transport_.GetTimestamp());
}

Status Musician::SetCustomInstrumentData(Id instrument_id,
                                         std::any custom_data) {
  return instrument_manager_.SetCustomData(
      instrument_id, transport_.GetTimestamp(), std::move(custom_data));
}

void Musician::SetConductor(ConductorDefinition definition,
                            ParamDefinitions param_definitions) {
  conductor_ = Conductor{std::move(definition), std::move(param_definitions)};
}

Status Musician::SetInstrumentNoteOff(Id instrument_id, float note_pitch) {
  return instrument_manager_.SetNoteOff(instrument_id,
                                        transport_.GetTimestamp(), note_pitch);
}

void Musician::SetInstrumentNoteOffCallback(
    InstrumentNoteOffCallback instrument_note_off_callback) {
  instrument_note_off_callback_ = instrument_note_off_callback
                                      ? std::move(instrument_note_off_callback)
                                      : &NoopInstrumentNoteOffCallback;
}

Status Musician::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                     float note_intensity) {
  return instrument_manager_.SetNoteOn(instrument_id, transport_.GetTimestamp(),
                                       note_pitch, note_intensity);
}

Status Musician::SetInstrumentParam(Id instrument_id, int param_id,
                                    float param_value) {
  return instrument_manager_.SetParam(instrument_id, transport_.GetTimestamp(),
                                      param_id, param_value);
}

Status Musician::SetInstrumentParamToDefault(Id instrument_id, int param_id) {
  return instrument_manager_.SetParamToDefault(
      instrument_id, transport_.GetTimestamp(), param_id);
}

void Musician::SetInstrumentNoteOnCallback(
    InstrumentNoteOnCallback instrument_note_on_callback) {
  instrument_note_on_callback_ = instrument_note_on_callback
                                     ? std::move(instrument_note_on_callback)
                                     : &NoopInstrumentNoteOnCallback;
}

Status Musician::SetPerformerBeginOffset(Id performer_id, double begin_offset) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerBeginPosition(
    Id performer_id, std::optional<double> begin_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceBeginPosition(std::move(begin_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerEndPosition(Id performer_id,
                                         std::optional<double> end_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceEndPosition(std::move(end_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoop(Id performer_id, bool loop) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoop(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoopBeginOffset(Id performer_id,
                                             double loop_begin_offset) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Musician::SetPerformerLoopLength(Id performer_id, double loop_length) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Musician::SetPlaybackBeatCallback(
    PlaybackBeatCallback playback_beat_callback) {
  transport_.SetBeatCallback(std::move(playback_beat_callback));
}

void Musician::SetPlaybackPosition(double position) {
  transport_.SetPosition(position);
}

void Musician::SetPlaybackTempo(double tempo) {
  playback_tempo_ = std::max(tempo, 0.0);
}

void Musician::SetPlaybackUpdateCallback(
    PlaybackUpdateCallback playback_update_callback) {
  playback_update_callback_ = playback_update_callback
                                  ? std::move(playback_update_callback)
                                  : &NoopPlaybackUpdateCallback;
}

void Musician::SetSampleRate(int sample_rate) {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  instrument_manager_.SetSampleRate(transport_.GetTimestamp(), sample_rate);
}

void Musician::StartPlayback() { transport_.Start(); }

void Musician::StopPlayback() {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  transport_.Stop();
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

void Musician::Update(double timestamp) {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barely
