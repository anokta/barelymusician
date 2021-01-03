#include "barelymusician/engine/engine.h"

#include <algorithm>

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

}  // namespace

Engine::Engine()
    : is_playing_(false),
      last_timestamp_(0.0),
      position_(0.0),
      tempo_(0.0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {}

StatusOr<InstrumentId> Engine::Create(
    InstrumentDefinition instrument,
    std::vector<InstrumentParamDefinition> params) {
  const auto instrument_id = ++id_counter_;
  controllers_.emplace(instrument_id, InstrumentController{params});
  task_runner_.Add([this, instrument_id, instrument, params]() {
    auto processor = InstrumentProcessor(instrument);
    for (const auto& param : params) {
      // TODO: min/max check (from controller).
      processor.SetParam(param.id, param.default_value);
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  return instrument_id;
}

Status Engine::Destroy(InstrumentId instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetParam(InstrumentId instrument_id,
                                 int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto* param = FindOrNull(controller->params, param_id)) {
      return param->value;
    }
  }
  return Status::kNotFound;
}

double Engine::GetPosition() const { return position_; }

double Engine::GetTempo() const { return tempo_; }

StatusOr<bool> Engine::IsNoteOn(InstrumentId instrument_id,
                                float note_index) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->notes.find(note_index) != controller->notes.cend();
  }
  return Status::kNotFound;
}

bool Engine::IsPlaying() const { return is_playing_; }

void Engine::AllNotesOff() {
  for (auto& [instrument_id, controller] : controllers_) {
    AllNotesOff(instrument_id);
  }
}

Status Engine::AllNotesOff(InstrumentId instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    std::vector<float> notes{controller->notes.begin(),
                             controller->notes.end()};
    controller->notes.clear();
    if (note_off_callback_) {
      for (const float note : notes) {
        note_off_callback_(last_timestamp_, instrument_id, note);
      }
    }
    task_runner_.Add([this, instrument_id, timestamp = last_timestamp_,
                      notes = std::move(notes)]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const float note : notes) {
          processor->ScheduleNoteOff(timestamp, note);
        }
      }
    });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::NoteOff(InstrumentId instrument_id, float note_index) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->notes.erase(note_index) > 0) {
      if (note_off_callback_) {
        note_off_callback_(last_timestamp_, instrument_id, note_index);
      }
      task_runner_.Add([this, instrument_id, note_index]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetNoteOff(note_index);
        }
      });
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::NoteOn(InstrumentId instrument_id, float note_index,
                      float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->notes.emplace(note_index).second) {
      if (note_on_callback_) {
        note_on_callback_(last_timestamp_, instrument_id, note_index,
                          note_intensity);
      }
      task_runner_.Add([this, instrument_id, note_index, note_intensity]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetNoteOn(note_index, note_intensity);
        }
      });
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetCustomData(InstrumentId instrument_id, void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    task_runner_.Add([this, instrument_id, custom_data]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        processor->SetCustomData(custom_data);
      }
    });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::Process(InstrumentId instrument_id, double begin_timestamp,
                       double end_timestamp, float* output, int num_channels,
                       int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(begin_timestamp, end_timestamp, output, num_channels,
                       num_frames);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ResetAllParams(InstrumentId instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    for (auto& [id, param] : controller->params) {
      param.value = param.default_value;
    }
    task_runner_.Add([this, instrument_id, params = controller->params]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& [id, param] : params) {
          processor->SetParam(id, param.default_value);
        }
      }
    });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetParam(InstrumentId instrument_id, int param_id,
                        float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (auto* param = FindOrNull(controller->params, param_id)) {
      if (param->value != param_value) {
        param->value = param_value;
        task_runner_.Add([this, instrument_id, param_id, param_value]() {
          if (auto* processor = FindOrNull(processors_, instrument_id)) {
            processor->SetParam(param_id, param_value);
          }
        });
        return Status::kOk;
      }
      return Status::kInvalidArgument;
    }
  }
  return Status::kNotFound;
}

void Engine::ClearAllScheduledNotes() {
  for (auto& [instrument_id, controller] : controllers_) {
    ClearAllScheduledNotes(instrument_id);
  }
}

Status Engine::ClearAllScheduledNotes(InstrumentId instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->messages.clear();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ScheduleNote(InstrumentId instrument_id, double position,
                            double duration, float note_index,
                            float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (position_ <= position && duration >= 0.0) {
      controller->messages.emplace(position,
                                   NoteOnData{note_index, note_intensity});
      controller->messages.emplace(position + duration,
                                   NoteOffData{note_index});
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

void Engine::SetPosition(double position) {
  if (position_ < position) {
    for (auto& [instrument_id, controller] : controllers_) {
      controller.messages.erase(controller.messages.cbegin(),
                                controller.messages.lower_bound(position));
    }
  }
  position_ = position;
}

void Engine::SetTempo(double tempo) { tempo_ = tempo; }

void Engine::Start(double timestamp) {
  last_timestamp_ = timestamp;
  is_playing_ = true;
}

void Engine::Stop() {
  is_playing_ = false;
  AllNotesOff();
}

void Engine::Update(double timestamp) {
  if (!is_playing_ || tempo_ <= 0.0 || timestamp <= last_timestamp_) {
    return;
  }

  const double end_position =
      position_ + BeatsFromSeconds(tempo_, timestamp - last_timestamp_);

  // Trigger beats.
  if (beat_callback_) {
    for (double beat = std::ceil(position_); beat < end_position; ++beat) {
      const double beat_timestamp =
          last_timestamp_ + SecondsFromBeats(tempo_, beat - position_);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }
  // Trigger messages.
  for (auto& controller_it : controllers_) {
    const InstrumentId& instrument_id = controller_it.first;
    InstrumentController& controller = controller_it.second;
    const auto cbegin = controller.messages.lower_bound(position_);
    const auto cend = controller.messages.lower_bound(end_position);
    for (auto it = cbegin; it != cend; ++it) {
      const double message_timestamp =
          last_timestamp_ + SecondsFromBeats(tempo_, it->first - position_);
      const auto message_data = it->second;
      std::visit(MessageDataVisitor{
                     [&](const NoteOffData& data) {
                       if (controller.notes.erase(data.index) > 0) {
                         if (note_off_callback_) {
                           note_off_callback_(message_timestamp, instrument_id,
                                              data.index);
                         }
                         task_runner_.Add(
                             [this, instrument_id, message_timestamp, data]() {
                               if (auto* processor =
                                       FindOrNull(processors_, instrument_id)) {
                                 processor->ScheduleNoteOff(message_timestamp,
                                                            data.index);
                               }
                             });
                       }
                     },
                     [&](const NoteOnData& data) {
                       if (controller.notes.emplace(data.index).second) {
                         if (note_on_callback_) {
                           note_on_callback_(message_timestamp, instrument_id,
                                             data.index, data.intensity);
                         }
                         task_runner_.Add([this, instrument_id,
                                           message_timestamp, data]() {
                           if (auto* processor =
                                   FindOrNull(processors_, instrument_id)) {
                             processor->ScheduleNoteOn(
                                 message_timestamp, data.index, data.intensity);
                           }
                         });
                       }
                     }},
                 message_data);
    }
    controller.messages.erase(cbegin, cend);
  }

  last_timestamp_ = timestamp;
  position_ = end_position;
}

Engine::InstrumentController::InstrumentController(
    const std::vector<InstrumentParamDefinition>& default_params) {
  for (const auto& [id, default_value] : default_params) {
    // TODO: std::min(std::max(default_value, min_value), max_value)
    const float value = default_value;
    params.emplace(id, InstrumentParam{value, value});
  }
}

}  // namespace barelyapi
