#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>
#include <variant>

#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/note_utils.h"
#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Returns number of seconds for the given number of |beats| and |tempo|.
//
// @param tempo Tempo in BPM.
// @param beats Number of beats.
// @return Number of seconds.
double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

std::int64_t SamplesFromBeats(int sample_rate, double tempo, double beats) {
  return SamplesFromSeconds(sample_rate, SecondsFromBeats(tempo, beats));
}

}  // namespace

Engine::Engine()
    : is_playing_(false), last_timestamp_(0), beat_callback_(nullptr) {}

int Engine::CreateInstrument(InstrumentDefinition definition,
                             InstrumentParamDefinitions param_definitions) {
  const int instrument_id =
      manager_.Create(definition, param_definitions, last_timestamp_);
  scores_.emplace(instrument_id, Score{});
  return instrument_id;
}

Status Engine::DestroyInstrument(int instrument_id) {
  if (scores_.erase(instrument_id) > 0) {
    manager_.Destroy(instrument_id, last_timestamp_);
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetParam(int instrument_id, int param_id) const {
  return manager_.GetParam(instrument_id, param_id);
}

double Engine::GetPosition() const { return clock_.GetPosition(); }

double Engine::GetTempo() const { return clock_.GetTempo(); }

StatusOr<bool> Engine::IsNoteOn(int instrument_id, float pitch) const {
  return manager_.IsNoteOn(instrument_id, pitch);
}

bool Engine::IsPlaying() const { return is_playing_; }

Status Engine::Process(int instrument_id, std::int64_t timestamp, float* output,
                       int num_channels, int num_frames) {
  return manager_.Process(instrument_id, timestamp, output, num_channels,
                          num_frames);
}

Status Engine::ResetAllParams(int instrument_id) {
  return manager_.ResetAllParams(instrument_id, last_timestamp_);
}

Status Engine::ResetParam(int instrument_id, int param_id) {
  return manager_.ResetParam(instrument_id, last_timestamp_, param_id);
}

void Engine::SetAllNotesOff() { manager_.SetAllNotesOff(last_timestamp_); }

Status Engine::SetAllNotesOff(int instrument_id) {
  return manager_.SetAllNotesOff(instrument_id, last_timestamp_);
}

Status Engine::SetCustomData(int instrument_id, void* custom_data) {
  return manager_.SetCustomData(instrument_id, last_timestamp_, custom_data);
}

Status Engine::SetNoteOff(int instrument_id, float pitch) {
  return manager_.SetNoteOff(instrument_id, last_timestamp_, pitch);
}

Status Engine::SetNoteOn(int instrument_id, float pitch, float intensity) {
  return manager_.SetNoteOn(instrument_id, last_timestamp_, pitch, intensity);
}

Status Engine::SetParam(int instrument_id, int param_id, float param_value) {
  return manager_.SetParam(instrument_id, last_timestamp_, param_id,
                           param_value);
}

void Engine::ClearAllScheduledNotes() {
  for (const auto& [instrument_id, score] : scores_) {
    ClearAllScheduledNotes(instrument_id);
  }
}

Status Engine::ClearAllScheduledNotes(int instrument_id) {
  if (auto* score = FindOrNull(scores_, instrument_id)) {
    score->RemoveAllEvents();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ScheduleNote(int instrument_id, double position, double duration,
                            float pitch, float intensity) {
  if (auto* score = FindOrNull(scores_, instrument_id)) {
    score->AddNoteEvent(position, duration, pitch, intensity);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Engine::SetNoteOffCallback(InstrumentNoteOffCallback note_off_callback) {
  manager_.SetNoteOffCallback(std::move(note_off_callback));
}

void Engine::SetNoteOnCallback(InstrumentNoteOnCallback note_on_callback) {
  manager_.SetNoteOnCallback(std::move(note_on_callback));
}

void Engine::SetPosition(double position) { clock_.SetPosition(position); }

void Engine::SetTempo(double tempo) { clock_.SetTempo(tempo); }

void Engine::Start(std::int64_t timestamp) {
  last_timestamp_ = timestamp;
  is_playing_ = true;
}

void Engine::Stop() {
  is_playing_ = false;
  SetAllNotesOff();
}

void Engine::Update(int sample_rate, std::int64_t timestamp) {
  const double tempo = clock_.GetTempo();
  if (!is_playing_ || tempo <= 0.0 || timestamp <= last_timestamp_) {
    return;
  }

  const double begin_position = clock_.GetPosition();
  clock_.UpdatePosition(
      SecondsFromSamples(sample_rate, timestamp - last_timestamp_));
  const double end_position = clock_.GetPosition();

  // Trigger beats.
  if (beat_callback_) {
    for (double beat = std::ceil(begin_position); beat < end_position; ++beat) {
      const std::int64_t beat_timestamp =
          last_timestamp_ +
          SamplesFromBeats(sample_rate, tempo, beat - end_position);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }
  // Trigger messages.
  for (auto& [id, score] : scores_) {
    score.ForEachEventInRange(
        begin_position, end_position,
        [&, id = id](double note_position, const InstrumentData& data) {
          const std::int64_t note_timestamp =
              last_timestamp_ +
              SamplesFromBeats(sample_rate, tempo,
                               note_position - begin_position);
          std::visit(InstrumentDataVisitor{
                         [&](const NoteOff& note_off) {
                           manager_.SetNoteOff(id, note_timestamp,
                                               note_off.pitch);
                         },
                         [&](const NoteOn& note_on) {
                           manager_.SetNoteOn(id, note_timestamp, note_on.pitch,
                                              note_on.intensity);
                         },
                         [](const auto&) {}},
                     data);
        });
  }
  last_timestamp_ = timestamp;
}

}  // namespace barelyapi
