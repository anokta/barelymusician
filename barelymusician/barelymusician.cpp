#include "barelymusician/barelymusician.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

namespace {

// Converts seconds from minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

BarelyMusician::BarelyMusician(int sample_rate)
    : instrument_manager_(sample_rate) {}

Id BarelyMusician::CreateInstrument(
    InstrumentDefinition definition,
    InstrumentParamDefinitions param_definitions) {
  const Id instrument_id = id_generator_.Generate();
  DCHECK(IsOk(instrument_manager_.Create(
      instrument_id, transport_.GetTimestamp(), std::move(definition),
      std::move(param_definitions))));
  return instrument_id;
}

Status BarelyMusician::DestroyInstrument(Id instrument_id) {
  return instrument_manager_.Destroy(instrument_id, transport_.GetTimestamp());
}

double BarelyMusician::GetPlaybackPosition() const {
  return transport_.GetPosition();
}

double BarelyMusician::GetPlaybackTempo() const {
  return transport_.GetTempo() * kSecondsFromMinutes;
}

bool BarelyMusician::IsPlaying() const { return transport_.IsPlaying(); }

void BarelyMusician::ProcessInstrument(Id instrument_id, double timestamp,
                                       float* output, int num_channels,
                                       int num_frames) {
  instrument_manager_.Process(instrument_id, timestamp, output, num_channels,
                              num_frames);
}

Status BarelyMusician::SetInstrumentNoteOff(Id instrument_id,
                                            float note_pitch) {
  return instrument_manager_.SetNoteOff(instrument_id,
                                        transport_.GetTimestamp(), note_pitch);
}

Status BarelyMusician::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                           float note_intensity) {
  return instrument_manager_.SetNoteOn(instrument_id, transport_.GetTimestamp(),
                                       note_pitch, note_intensity);
}

void BarelyMusician::SetPlaybackBeatCallback(
    PlaybackBeatCallback playback_beat_callback) {
  transport_.SetBeatCallback(std::move(playback_beat_callback));
}

void BarelyMusician::SetPlaybackPosition(double position) {
  transport_.SetPosition(position);
}

void BarelyMusician::SetPlaybackTempo(double tempo) {
  transport_.SetTempo(std::max(tempo, 0.0) * kMinutesFromSeconds);
}

void BarelyMusician::StartPlayback() { transport_.Start(); }

void BarelyMusician::StopPlayback() { transport_.Stop(); }

void BarelyMusician::Update(double timestamp) {
  transport_.Update(timestamp);
  instrument_manager_.Update();
}

}  // namespace barelyapi
