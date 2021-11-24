#include "barelymusician/barelymusician.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param_definition.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

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

BarelyMusician::BarelyMusician(int sample_rate)
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

Id BarelyMusician::AddInstrument(InstrumentDefinition definition,
                                 InstrumentParamDefinitions param_definitions) {
  const Id instrument_id = id_generator_.Generate();
  instrument_manager_.Add(instrument_id, transport_.GetTimestamp(),
                          std::move(definition), std::move(param_definitions));
  return instrument_id;
}

Id BarelyMusician::AddPerformer() {
  const Id performer_id = id_generator_.Generate();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

Status BarelyMusician::AddPerformerInstrument(Id performer_id,
                                              Id instrument_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (instrument_manager_.IsValid(instrument_id)) {
      return performer->AddInstrument(instrument_id);
    }
  }
  return Status::kNotFound;
}

StatusOr<Id> BarelyMusician::AddPerformerNote(Id performer_id, double position,
                                              Note note) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Generate();
    performer->GetMutableSequence()->AddNote(note_id, position,
                                             std::move(note));
    return note_id;
  }
  return Status::kNotFound;
}

StatusOr<double> BarelyMusician::GetPerformerBeginOffset(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<std::optional<double>> BarelyMusician::GetPerformerBeginPosition(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequenceBeginPosition();
  }
  return Status::kNotFound;
}

StatusOr<std::optional<double>> BarelyMusician::GetPerformerEndPosition(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequenceEndPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> BarelyMusician::GetPerformerLoopBeginOffset(
    Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> BarelyMusician::GetPerformerLoopLength(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().GetLoopLength();
  }
  return Status::kNotFound;
}

double BarelyMusician::GetPlaybackPosition() const {
  return transport_.GetPosition();
}

double BarelyMusician::GetPlaybackTempo() const { return playback_tempo_; }

StatusOr<bool> BarelyMusician::IsPerformerEmpty(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> BarelyMusician::IsPerformerLooping(Id performer_id) const {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetSequence().IsLooping();
  }
  return Status::kNotFound;
}

bool BarelyMusician::IsPlaying() const { return transport_.IsPlaying(); }

void BarelyMusician::ProcessInstrument(Id instrument_id, double timestamp,
                                       float* output, int num_channels,
                                       int num_frames) {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
}

Status BarelyMusician::RemoveAllPerformerInstruments(Id performer_id) {
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

Status BarelyMusician::RemoveAllPerformerNotes(Id performer_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::RemoveAllPerformerNotes(Id performer_id,
                                               double begin_position,
                                               double end_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->RemoveAllNotes(begin_position,
                                                    end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::RemoveInstrument(Id instrument_id) {
  const auto status =
      instrument_manager_.Remove(instrument_id, transport_.GetTimestamp());
  if (IsOk(status)) {
    for (auto& [performer_id, performer] : performers_) {
      performer.RemoveInstrument(instrument_id);
    }
  }
  return status;
}

Status BarelyMusician::RemovePerformer(Id performer_id) {
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

Status BarelyMusician::RemovePerformerInstrument(Id performer_id,
                                                 Id instrument_id) {
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

Status BarelyMusician::RemovePerformerNote(Id performer_id, Id note_id) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->GetMutableSequence()->RemoveNote(note_id);
  }
  return Status::kNotFound;
}

void BarelyMusician::SetAllInstrumentNotesOff() {
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

Status BarelyMusician::SetAllInstrumentNotesOff(Id instrument_id) {
  return instrument_manager_.SetAllNotesOff(instrument_id,
                                            transport_.GetTimestamp());
}

void BarelyMusician::SetAllInstrumentParamsToDefault() {
  instrument_manager_.SetAllParamsToDefault(transport_.GetTimestamp());
}

Status BarelyMusician::SetAllInstrumentParamsToDefault(Id instrument_id) {
  return instrument_manager_.SetAllParamsToDefault(instrument_id,
                                                   transport_.GetTimestamp());
}

void BarelyMusician::SetConductor(ConductorDefinition definition) {
  conductor_ = Conductor{std::move(definition)};
}

Status BarelyMusician::SetInstrumentNoteOff(Id instrument_id,
                                            float note_pitch) {
  return instrument_manager_.SetNoteOff(instrument_id,
                                        transport_.GetTimestamp(), note_pitch);
}

void BarelyMusician::SetInstrumentNoteOffCallback(
    InstrumentNoteOffCallback instrument_note_off_callback) {
  instrument_note_off_callback_ = instrument_note_off_callback
                                      ? std::move(instrument_note_off_callback)
                                      : &NoopInstrumentNoteOffCallback;
}

Status BarelyMusician::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                           float note_intensity) {
  return instrument_manager_.SetNoteOn(instrument_id, transport_.GetTimestamp(),
                                       note_pitch, note_intensity);
}

Status BarelyMusician::SetInstrumentParam(Id instrument_id, int param_id,
                                          float param_value) {
  return instrument_manager_.SetParam(instrument_id, transport_.GetTimestamp(),
                                      param_id, param_value);
}

Status BarelyMusician::SetInstrumentParamToDefault(Id instrument_id,
                                                   int param_id) {
  return instrument_manager_.SetParamToDefault(
      instrument_id, transport_.GetTimestamp(), param_id);
}

void BarelyMusician::SetInstrumentNoteOnCallback(
    InstrumentNoteOnCallback instrument_note_on_callback) {
  instrument_note_on_callback_ = instrument_note_on_callback
                                     ? std::move(instrument_note_on_callback)
                                     : &NoopInstrumentNoteOnCallback;
}

Status BarelyMusician::SetPerformerBeginOffset(Id performer_id,
                                               double begin_offset) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::SetPerformerBeginPosition(
    Id performer_id, std::optional<double> begin_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceBeginPosition(std::move(begin_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::SetPerformerEndPosition(
    Id performer_id, std::optional<double> end_position) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->SetSequenceEndPosition(std::move(end_position));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::SetPerformerLoopBeginOffset(Id performer_id,
                                                   double loop_begin_offset) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::SetPerformerLoopLength(Id performer_id,
                                              double loop_length) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status BarelyMusician::SetPerformerLooping(Id performer_id, bool looping) {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->GetMutableSequence()->SetLooping(looping);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void BarelyMusician::SetPlaybackBeatCallback(
    PlaybackBeatCallback playback_beat_callback) {
  transport_.SetBeatCallback(std::move(playback_beat_callback));
}

void BarelyMusician::SetPlaybackPosition(double position) {
  transport_.SetPosition(position);
}

void BarelyMusician::SetPlaybackTempo(double tempo) {
  playback_tempo_ = std::max(tempo, 0.0);
}

void BarelyMusician::SetPlaybackUpdateCallback(
    PlaybackUpdateCallback playback_update_callback) {
  playback_update_callback_ = playback_update_callback
                                  ? std::move(playback_update_callback)
                                  : &NoopPlaybackUpdateCallback;
}

void BarelyMusician::StartPlayback() { transport_.Start(); }

void BarelyMusician::StopPlayback() {
  for (auto& [performer_id, performer] : performers_) {
    performer.ClearAllActiveNotes();
  }
  transport_.Stop();
  instrument_manager_.SetAllNotesOff(transport_.GetTimestamp());
}

void BarelyMusician::Update(double timestamp) {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barelyapi
