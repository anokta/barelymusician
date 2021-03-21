#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

namespace barelyapi {

Engine::Engine(int sample_rate)
    : manager_(sample_rate, &id_generator_), sequencer_(&manager_) {}

Id Engine::CreateInstrument(InstrumentDefinition definition,
                            InstrumentParamDefinitions param_definitions) {
  return manager_.Create(std::move(definition), std::move(param_definitions));
}

bool Engine::DestroyInstrument(Id instrument_id) {
  return manager_.Destroy(instrument_id);
}

std::vector<float> Engine::GetAllInstrumentNotes(Id instrument_id) const {
  return manager_.GetAllNotes(instrument_id);
}

std::vector<Param> Engine::GetAllInstrumentParams(Id instrument_id) const {
  return manager_.GetAllParams(instrument_id);
}

const float* Engine::GetInstrumentParam(Id instrument_id, int param_id) const {
  return manager_.GetParam(instrument_id, param_id);
}

double Engine::GetPlaybackPosition() const {
  return sequencer_.GetPlaybackPosition();
}

double Engine::GetPlaybackTempo() const {
  return sequencer_.GetPlaybackTempo();
}

bool Engine::IsInstrumentNoteOn(Id instrument_id, float note_pitch) const {
  return manager_.IsNoteOn(instrument_id, note_pitch);
}

bool Engine::IsPlaying() const { return sequencer_.IsPlaying(); }

bool Engine::ProcessInstrument(Id instrument_id, double timestamp,
                               float* output, int num_channels,
                               int num_frames) {
  return manager_.Process(instrument_id, output, num_channels, num_frames,
                          timestamp);
}

void Engine::RemoveAllScheduledInstrumentNotes() {
  sequencer_.RemoveAllScheduledInstrumentNotes();
}

void Engine::RemoveAllScheduledInstrumentNotes(Id instrument_id) {
  sequencer_.RemoveAllScheduledInstrumentNotes(instrument_id);
}

void Engine::ResetAllInstrumentParams() { manager_.ResetAllParams(); }

bool Engine::ResetAllInstrumentParams(Id instrument_id) {
  return manager_.ResetAllParams(instrument_id);
}

bool Engine::ResetInstrumentParam(Id instrument_id, int param_id) {
  return manager_.ResetParam(instrument_id, param_id);
}

bool Engine::ScheduleInstrumentNote(Id instrument_id,
                                    double note_begin_position,
                                    double note_end_position, float note_pitch,
                                    float note_intensity) {
  sequencer_.ScheduleInstrumentNote(instrument_id, note_begin_position,
                                    note_end_position, note_pitch,
                                    note_intensity);
  return true;
}

void Engine::SetAllInstrumentNotesOff() { manager_.SetAllNotesOff(); }

bool Engine::SetAllInstrumentNotesOff(Id instrument_id) {
  return manager_.SetAllNotesOff(instrument_id);
}

void Engine::SetBeatCallback(BeatCallback beat_callback) {
  sequencer_.SetBeatCallback(std::move(beat_callback));
}

bool Engine::SetCustomInstrumentData(Id instrument_id, void* custom_data) {
  return manager_.SetCustomData(instrument_id, custom_data);
}

bool Engine::SetInstrumentNoteOff(Id instrument_id, float note_pitch) {
  return manager_.SetNoteOff(instrument_id, note_pitch);
}

bool Engine::SetInstrumentNoteOn(Id instrument_id, float note_pitch,
                                 float note_intensity) {
  return manager_.SetNoteOn(instrument_id, note_pitch, note_intensity);
}

bool Engine::SetInstrumentParam(Id instrument_id, int param_id,
                                float param_value) {
  return manager_.SetParam(instrument_id, param_id, param_value);
}

void Engine::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  manager_.SetNoteOffCallback(std::move(note_off_callback));
}
void Engine::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  manager_.SetNoteOnCallback(std::move(note_on_callback));
}

void Engine::SetPlaybackPosition(double position) {
  // manager_.SetAllNotesOff();
  sequencer_.SetPlaybackPosition(position);
}

void Engine::SetPlaybackTempo(double tempo) {
  sequencer_.SetPlaybackTempo(tempo);
}

void Engine::SetSampleRate(int sample_rate) {
  manager_.SetSampleRate(sample_rate);
}

void Engine::StartPlayback() { sequencer_.StartPlayback(); }

void Engine::StopPlayback() {
  // manager_.SetAllNotesOff();
  sequencer_.StopPlayback();
}

void Engine::Update(double timestamp) {
  sequencer_.Update(timestamp);
  manager_.Update(timestamp);
}

}  // namespace barelyapi
