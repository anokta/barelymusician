#include "barelymusician/engine/engine.h"

#include <algorithm>

#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Returns number of beats for the given number of |seconds| and |tempo|.
//
// @param tempo Tempo in BPM.
// @param seconds Number of seconds.
// @return Number of beats.
double BeatsFromSeconds(double tempo, double seconds) {
  return tempo * seconds / kSecondsFromMinutes;
}

// Returns number of seconds for the given number of |beats| and |tempo|.
//
// @param tempo Tempo in BPM.
// @param beats Number of beats.
// @return Number of seconds.
double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

double BeatsFromSamples(int sample_rate, double tempo, std::int64_t samples) {
  return BeatsFromSeconds(tempo, SecondsFromSamples(sample_rate, samples));
}

std::int64_t SamplesFromBeats(int sample_rate, double tempo, double beats) {
  return SamplesFromSeconds(sample_rate, SecondsFromBeats(tempo, beats));
}

}  // namespace

Engine::Engine()
    : is_playing_(false),
      last_timestamp_(0),
      position_(0.0),
      tempo_(0.0),
      beat_callback_(nullptr) {}

std::int64_t Engine::Create(InstrumentDefinition definition,
                            InstrumentParamDefinitions param_definitions) {
  const std::int64_t instrument_id =
      manager_.Create(definition, param_definitions, last_timestamp_);
  scores_.emplace(instrument_id, std::multimap<double, InstrumentData>{});
  return instrument_id;
}

Status Engine::Destroy(std::int64_t instrument_id) {
  if (scores_.erase(instrument_id) > 0) {
    manager_.Destroy(instrument_id, last_timestamp_);
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetParam(std::int64_t instrument_id,
                                 int param_id) const {
  return manager_.GetParam(instrument_id, param_id);
}

double Engine::GetPosition() const { return position_; }

double Engine::GetTempo() const { return tempo_; }

StatusOr<bool> Engine::IsNoteOn(std::int64_t instrument_id, float pitch) const {
  return manager_.IsNoteOn(instrument_id, pitch);
}

bool Engine::IsPlaying() const { return is_playing_; }

Status Engine::Process(std::int64_t instrument_id, std::int64_t timestamp,
                       float* output, int num_channels, int num_frames) {
  return manager_.Process(instrument_id, timestamp, output, num_channels,
                          num_frames);
}

Status Engine::ResetAllParams(std::int64_t instrument_id) {
  return manager_.ResetAllParams(instrument_id, last_timestamp_);
}

Status Engine::ResetParam(std::int64_t instrument_id, int param_id) {
  return manager_.ResetParam(instrument_id, last_timestamp_, param_id);
}

void Engine::SetAllNotesOff() { manager_.SetAllNotesOff(last_timestamp_); }

Status Engine::SetAllNotesOff(std::int64_t instrument_id) {
  return manager_.SetAllNotesOff(instrument_id, last_timestamp_);
}

Status Engine::SetCustomData(std::int64_t instrument_id, void* custom_data) {
  return manager_.SetCustomData(instrument_id, last_timestamp_, custom_data);
}

Status Engine::SetNoteOff(std::int64_t instrument_id, float pitch) {
  return manager_.SetNoteOff(instrument_id, last_timestamp_, pitch);
}

Status Engine::SetNoteOn(std::int64_t instrument_id, float pitch,
                         float intensity) {
  return manager_.SetNoteOn(instrument_id, last_timestamp_, pitch, intensity);
}

Status Engine::SetParam(std::int64_t instrument_id, int param_id,
                        float param_value) {
  return manager_.SetParam(instrument_id, last_timestamp_, param_id,
                           param_value);
}

void Engine::ClearAllScheduledNotes() {
  for (const auto& [instrument_id, score] : scores_) {
    ClearAllScheduledNotes(instrument_id);
  }
}

Status Engine::ClearAllScheduledNotes(std::int64_t instrument_id) {
  if (auto* score = FindOrNull(scores_, instrument_id)) {
    score->clear();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ScheduleNote(std::int64_t instrument_id, double position,
                            double duration, float pitch, float intensity) {
  if (auto* score = FindOrNull(scores_, instrument_id)) {
    if (position_ <= position && duration >= 0.0) {
      score->emplace(position, NoteOn{pitch, intensity});
      score->emplace(position + duration, NoteOff{pitch});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
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

void Engine::SetPosition(double position) { position_ = position; }

void Engine::SetTempo(double tempo) { tempo_ = tempo; }

void Engine::Start(std::int64_t timestamp) {
  last_timestamp_ = timestamp;
  is_playing_ = true;
}

void Engine::Stop() {
  is_playing_ = false;
  SetAllNotesOff();
}

void Engine::Update(int sample_rate, std::int64_t timestamp) {
  if (!is_playing_ || tempo_ <= 0.0 || timestamp <= last_timestamp_) {
    return;
  }

  const double end_position =
      position_ +
      BeatsFromSamples(sample_rate, tempo_, timestamp - last_timestamp_);

  // Trigger beats.
  if (beat_callback_) {
    for (double beat = std::ceil(position_); beat < end_position; ++beat) {
      const std::int64_t beat_timestamp =
          last_timestamp_ +
          SamplesFromBeats(sample_rate, tempo_, beat - position_);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }
  // Trigger messages.
  for (auto& [id, score] : scores_) {
    const auto cbegin = score.lower_bound(position_);
    const auto cend = score.lower_bound(end_position);
    for (auto it = cbegin; it != cend; ++it) {
      const std::int64_t message_timestamp =
          last_timestamp_ +
          SamplesFromBeats(sample_rate, tempo_, it->first - position_);
      const auto message_data = it->second;
      std::visit(InstrumentDataVisitor{
                     [&, id = id](const NoteOff& note_off) {
                       manager_.SetNoteOff(id, message_timestamp,
                                           note_off.pitch);
                     },
                     [&, id = id](const NoteOn& note_on) {
                       manager_.SetNoteOn(id, message_timestamp, note_on.pitch,
                                          note_on.intensity);
                     },
                     [](const auto&) {}},
                 message_data);
    }
  }

  last_timestamp_ = timestamp;
  position_ = end_position;
}

}  // namespace barelyapi
