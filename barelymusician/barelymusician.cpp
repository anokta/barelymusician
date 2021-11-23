#include "barelymusician/barelymusician.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
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

Id BarelyMusician::CreateInstrument(
    InstrumentDefinition definition,
    InstrumentParamDefinitions param_definitions) {
  const Id instrument_id = id_generator_.Generate();
  DCHECK(IsOk(instrument_manager_.Create(
      instrument_id, transport_.GetTimestamp(), std::move(definition),
      std::move(param_definitions))));
  return instrument_id;
}

Id BarelyMusician::CreatePerformer() {
  const Id performer_id = id_generator_.Generate();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

Status BarelyMusician::DestroyInstrument(Id instrument_id) {
  return instrument_manager_.Destroy(instrument_id, transport_.GetTimestamp());
}

Status BarelyMusician::DestroyPerformer(Id performer_id) {
  if (performers_.erase(performer_id) > 0) {
    return Status::kOk;
  }
  return Status::kNotFound;
}

double BarelyMusician::GetPlaybackPosition() const {
  return transport_.GetPosition();
}

double BarelyMusician::GetPlaybackTempo() const { return playback_tempo_; }

bool BarelyMusician::IsPlaying() const { return transport_.IsPlaying(); }

void BarelyMusician::ProcessInstrument(Id instrument_id, double timestamp,
                                       float* output, int num_channels,
                                       int num_frames) {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
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

void BarelyMusician::SetInstrumentNoteOnCallback(
    InstrumentNoteOnCallback instrument_note_on_callback) {
  instrument_note_on_callback_ = instrument_note_on_callback
                                     ? std::move(instrument_note_on_callback)
                                     : &NoopInstrumentNoteOnCallback;
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
