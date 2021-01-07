#include "barelymusician/engine/engine.h"

#include <algorithm>

#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each |Process| call.
constexpr int kNumMaxTasks = 1000;

// Returns a map value by key.
//
// @param map Map.
// @param key Key.
// @return Const pointer to value if succeeds, nullptr otherwise.
template <typename KeyType, typename ValueType>
const ValueType* FindOrNull(const std::unordered_map<KeyType, ValueType>& map,
                            const KeyType& key) {
  if (const auto it = map.find(key); it != map.cend()) {
    return &it->second;
  }
  return nullptr;
}

// Returns a map value by key.
//
// @param map Map.
// @param key Key.
// @return Pointer to value if succeeds, nullptr otherwise.
template <typename KeyType, typename ValueType>
ValueType* FindOrNull(std::unordered_map<KeyType, ValueType>& map,
                      const KeyType& key) {
  if (auto it = map.find(key); it != map.end()) {
    return &it->second;
  }
  return nullptr;
}

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

double BeatsFromSamples(int sample_rate, double tempo, int64 samples) {
  return BeatsFromSeconds(tempo, SecondsFromSamples(sample_rate, samples));
}

int64 SamplesFromBeats(int sample_rate, double tempo, double beats) {
  return SamplesFromSeconds(sample_rate, SecondsFromBeats(tempo, beats));
}

}  // namespace

Engine::Engine()
    : is_playing_(false),
      last_timestamp_(0),
      position_(0.0),
      tempo_(0.0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {}

StatusOr<int64> Engine::Create(InstrumentDefinition instrument,
                               std::vector<InstrumentParamDefinition> params) {
  const auto instrument_id = ++id_counter_;
  auto controller = InstrumentController{params};
  controllers_.emplace(instrument_id, std::move(controller));
  scores_.emplace(instrument_id, std::multimap<double, InstrumentData>{});
  task_runner_.Add([this, instrument_id, instrument, params]() {
    auto processor = InstrumentProcessor(instrument);
    for (const auto& param : params) {
      // TODO: min/max check (from controller).
      processor.SetData(last_timestamp_, Param{param.id, param.default_value});
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  return instrument_id;
}

Status Engine::Destroy(int64 instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    scores_.erase(instrument_id);
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetParam(int64 instrument_id, int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const float* value = controller->GetParam(param_id)) {
      return *value;
    }
  }
  return Status::kNotFound;
}

double Engine::GetPosition() const { return position_; }

double Engine::GetTempo() const { return tempo_; }

StatusOr<bool> Engine::IsNoteOn(int64 instrument_id, float pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(pitch);
  }
  return Status::kNotFound;
}

bool Engine::IsPlaying() const { return is_playing_; }

Status Engine::Process(int64 instrument_id, int64 timestamp, float* output,
                       int num_channels, int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ResetAllParams(int64 instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    for (const auto& [id, param] : controller->GetAllParams()) {
      ResetParam(instrument_id, id);
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ResetParam(int64 instrument_id, int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(param_id)) {
      task_runner_.Add([this, instrument_id, param_id,
                        param_value = *controller->GetParam(param_id)]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetData(last_timestamp_, Param{param_id, param_value});
        }
      });
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetAllNotesOff() {
  for (auto& [instrument_id, controller] : controllers_) {
    SetAllNotesOff(instrument_id);
  }
}

Status Engine::SetAllNotesOff(int64 instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    for (const float note : controller->GetAllNotes()) {
      SetNoteOff(instrument_id, note);
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetCustomData(int64 instrument_id, void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    task_runner_.Add([this, instrument_id, custom_data]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        processor->SetData(last_timestamp_, CustomData{custom_data});
      }
    });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetNoteOff(int64 instrument_id, float pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOff(pitch)) {
      if (note_off_callback_) {
        note_off_callback_(last_timestamp_, instrument_id, pitch);
      }
      task_runner_.Add([this, instrument_id, pitch]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetData(last_timestamp_, NoteOff{pitch});
        }
      });
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetNoteOn(int64 instrument_id, float pitch, float intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOn(pitch)) {
      if (note_on_callback_) {
        note_on_callback_(last_timestamp_, instrument_id, pitch, intensity);
      }
      task_runner_.Add([this, instrument_id, pitch, intensity]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetData(last_timestamp_, NoteOn{pitch, intensity});
        }
      });
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetParam(int64 instrument_id, int param_id, float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(param_id, param_value)) {
      task_runner_.Add([this, instrument_id, param_id,
                        param_value = *controller->GetParam(param_id)]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetData(last_timestamp_, Param{param_id, param_value});
        }
      });
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void Engine::ClearAllScheduledNotes() {
  for (auto& [instrument_id, controller] : controllers_) {
    ClearAllScheduledNotes(instrument_id);
  }
}

Status Engine::ClearAllScheduledNotes(int64 instrument_id) {
  if (auto* score = FindOrNull(scores_, instrument_id)) {
    score->clear();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ScheduleNote(int64 instrument_id, double position,
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

void Engine::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void Engine::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void Engine::SetPosition(double position) { position_ = position; }

void Engine::SetTempo(double tempo) { tempo_ = tempo; }

void Engine::Start(int64 timestamp) {
  last_timestamp_ = timestamp;
  is_playing_ = true;
}

void Engine::Stop() {
  is_playing_ = false;
  SetAllNotesOff();
}

void Engine::Update(int sample_rate, int64 timestamp) {
  if (!is_playing_ || tempo_ <= 0.0 || timestamp <= last_timestamp_) {
    return;
  }

  const double end_position =
      position_ +
      BeatsFromSamples(sample_rate, tempo_, timestamp - last_timestamp_);

  // Trigger beats.
  if (beat_callback_) {
    for (double beat = std::ceil(position_); beat < end_position; ++beat) {
      const int64 beat_timestamp =
          last_timestamp_ +
          SamplesFromBeats(sample_rate, tempo_, beat - position_);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }
  // Trigger messages.
  for (auto& [id, score] : scores_) {
    auto* controller = FindOrNull(controllers_, id);
    const auto cbegin = score.lower_bound(position_);
    const auto cend = score.lower_bound(end_position);
    for (auto it = cbegin; it != cend; ++it) {
      const int64 message_timestamp =
          last_timestamp_ +
          SamplesFromBeats(sample_rate, tempo_, it->first - position_);
      const auto message_data = it->second;
      std::visit(
          InstrumentDataVisitor{
              [](const auto&) {},
              [&, id = id](const NoteOff& note_off) {
                if (controller->SetNoteOff(note_off.pitch)) {
                  if (note_off_callback_) {
                    note_off_callback_(message_timestamp, id, note_off.pitch);
                  }
                  task_runner_.Add(
                      [this, id = id, message_timestamp, note_off]() {
                        if (auto* processor = FindOrNull(processors_, id)) {
                          processor->SetData(message_timestamp, note_off);
                        }
                      });
                }
              },
              [&, id = id](const NoteOn& note_on) {
                if (controller->SetNoteOn(note_on.pitch)) {
                  if (note_on_callback_) {
                    note_on_callback_(message_timestamp, id, note_on.pitch,
                                      note_on.intensity);
                  }
                  task_runner_.Add(
                      [this, id = id, message_timestamp, note_on]() {
                        if (auto* processor = FindOrNull(processors_, id)) {
                          processor->SetData(message_timestamp, note_on);
                        }
                      });
                }
              }},
          message_data);
    }
    score.erase(cbegin, cend);
  }

  last_timestamp_ = timestamp;
  position_ = end_position;
}

}  // namespace barelyapi
