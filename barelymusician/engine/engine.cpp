#include "barelymusician/engine/engine.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each Process call.
constexpr int kNumMaxTasks = 500;

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

double BeatsFromSeconds(double tempo, double seconds) {
  return tempo * seconds / kSecondsFromMinutes;
}

double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

}  // namespace

Engine::Engine()
    : is_playing_(false),
      position_(0.0),
      tempo_(0.0),
      last_timestamp_(0.0),
      conductor_(nullptr),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {}

Engine::Id Engine::Create(std::unique_ptr<Instrument> instrument,
                          std::vector<std::pair<int, float>> params) {
  const Id instrument_id = ++id_counter_;
  controllers_.emplace(instrument_id, Controller{InstrumentController{params}});
  task_runner_.Add([this, instrument_id,
                    instrument = std::make_shared<std::unique_ptr<Instrument>>(
                        std::move(instrument)),
                    params = std::move(params)]() {
    processors_.emplace(instrument_id,
                        InstrumentProcessor{std::move(*instrument), params});
  });
  return instrument_id;
}

bool Engine::Destroy(Id instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return true;
  }
  return false;
}

double Engine::GetPosition() const { return position_; }

double Engine::GetTempo() const { return tempo_; }

bool Engine::IsPlaying() const { return is_playing_; }

std::optional<float> Engine::GetParam(Id instrument_id, int id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->controller.GetParam(id);
  }
  return std::nullopt;
}

std::optional<bool> Engine::IsNoteOn(Id instrument_id, float index) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->controller.IsNoteOn(index);
  }
  return std::nullopt;
}

void Engine::AllNotesOff() {
  for (auto& [id, controller] : controllers_) {
    AllNotesOff(id);
  }
}

bool Engine::AllNotesOff(Id instrument_id) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (!controller) {
    return false;
  }
  controller->messages.Clear();
  task_runner_.Add([this, instrument_id, timestamp = last_timestamp_]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    DCHECK(processor);
    processor->Clear();
  });
  const auto notes = controller->controller.GetAllNotes();
  for (const auto& note : notes) {
    task_runner_.Add([this, instrument_id, note = note]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->NoteOff(last_timestamp_, note);
    });
  }
  if (note_off_callback_) {
    for (const auto& note : notes) {
      note_off_callback_(last_timestamp_, instrument_id, note);
    }
  }
  controller->controller.AllNotesOff();
  return true;
}

std::optional<bool> Engine::Control(Id instrument_id, int id, float value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto success_or = controller->controller.SetParam(id, value);
        success_or.value_or(false)) {
      task_runner_.Add([this, instrument_id, id, value]() {
        auto* processor = FindOrNull(processors_, instrument_id);
        DCHECK(processor);
        processor->SetParam(last_timestamp_, id, value);
      });
      return *success_or;
    }
  }
  return std::nullopt;
}

bool Engine::NoteOff(Id instrument_id, float index) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->controller.NoteOff(index)) {
      if (note_off_callback_) {
        note_off_callback_(last_timestamp_, instrument_id, index);
      }
      task_runner_.Add([this, instrument_id, index]() {
        auto* processor = FindOrNull(processors_, instrument_id);
        DCHECK(processor);
        processor->NoteOff(last_timestamp_, index);
      });
      return true;
    }
  }
  return false;
}

bool Engine::NoteOn(Id instrument_id, float index, float intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->controller.NoteOn(index)) {
      if (note_on_callback_) {
        note_on_callback_(last_timestamp_, instrument_id, index, intensity);
      }
      task_runner_.Add([this, instrument_id, index, intensity]() {
        auto* processor = FindOrNull(processors_, instrument_id);
        DCHECK(processor);
        processor->NoteOn(last_timestamp_, index, intensity);
      });
      return true;
    }
  }
  return false;
}

bool Engine::Process(Id instrument_id, double begin_timestamp,
                     double end_timestamp, float* output, int num_channels,
                     int num_frames) {
  task_runner_.Run();
  auto* processor = FindOrNull(processors_, instrument_id);
  if (!processor) {
    return false;
  }
  processor->Process(begin_timestamp, end_timestamp, output, num_channels,
                     num_frames);
  return true;
}

bool Engine::ResetAllParams(Id instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->controller.ResetAllParams();
    for (auto& [id, param] : controller->controller.GetAllParams()) {
      task_runner_.Add([this, instrument_id, id = id, param = param]() {
        auto* processor = FindOrNull(processors_, instrument_id);
        DCHECK(processor);
        processor->SetParam(last_timestamp_, id, param);
      });
    }
    return true;
  }
  return false;
}

bool Engine::ScheduleControl(Id instrument_id, double position, int id,
                             float value) {
  if (position < position_) {
    return false;
  }
  auto* controller = FindOrNull(controllers_, instrument_id);
  controller->messages.Push(position, ControlData{id, value});
  return true;
}

bool Engine::ScheduleNote(Id instrument_id, double position, double duration,
                          float index, float intensity) {
  DCHECK_GE(position, 0.0);
  DCHECK_GE(duration, 0.0);
  if (position < position_) {
    return false;
  }
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->messages.Push(position, NoteOnData{index, intensity});
    controller->messages.Push(position + duration, NoteOffData{index});
    return true;
  }
  return false;
}

bool Engine::ScheduleNoteOff(Id instrument_id, double position, float index) {
  DCHECK_GE(position, 0.0);
  if (position < position_) {
    return false;
  }
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->messages.Push(position, NoteOffData{index});
    return true;
  }
  return false;
}

bool Engine::ScheduleNoteOn(Id instrument_id, double position, float index,
                            float intensity) {
  DCHECK_GE(position, 0.0);
  if (position < position_) {
    return false;
  }
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->messages.Push(position, NoteOnData{index, intensity});
    return true;
  }
  return false;
}

void Engine::SetConductor(std::unique_ptr<Conductor> conductor) {
  conductor_ = std::move(conductor);
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
  DCHECK_GE(position, 0.0);
  position_ = position;
  for (auto& [id, controller] : controllers_) {
    controller.messages.Clear(controller.messages.GetIterator(position_));
  }
}

void Engine::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  tempo_ = tempo;
}

void Engine::Start(double timestamp) {
  DCHECK_GE(timestamp, 0.0);
  last_timestamp_ = timestamp;
  is_playing_ = true;
}

void Engine::Stop() { is_playing_ = false; }

void Engine::Update(double timestamp) {
  DCHECK_GE(timestamp, 0.0);
  if (!is_playing_ || tempo_ <= 0.0 || timestamp <= last_timestamp_) {
    return;
  }

  const double elapsed_beats =
      BeatsFromSeconds(tempo_, timestamp - last_timestamp_);
  const double end_position = position_ + elapsed_beats;

  // Trigger beats.
  if (beat_callback_) {
    for (double beat = std::ceil(position_); beat < end_position; ++beat) {
      const double beat_timestamp =
          last_timestamp_ + SecondsFromBeats(tempo_, beat - position_);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }
  // Trigger messages.
  for (auto& [id, controller] : controllers_) {
    const auto messages = controller.messages.GetIterator(end_position);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const auto message_data = it->data;
      const double message_timestamp =
          last_timestamp_ + SecondsFromBeats(tempo_, it->timestamp - position_);
      std::visit(
          MessageVisitor{
              [this, id = id,
               controller = &controller.controller](const ControlData& data) {
                controller->SetParam(data.id, data.value);
              },
              [this, id = id, controller = &controller.controller,
               message_timestamp](const NoteOffData& data) {
                if (controller->NoteOff(data.index) && note_off_callback_) {
                  note_off_callback_(message_timestamp, id, data.index);
                }
              },
              [this, id = id, controller = &controller.controller,
               message_timestamp](const NoteOnData& data) {
                if (controller->NoteOn(data.index) && note_on_callback_) {
                  note_on_callback_(message_timestamp, id, data.index,
                                    data.intensity);
                }
              }},
          message_data);
      task_runner_.Add([this, id = id, message_data, message_timestamp]() {
        auto* processor = FindOrNull(processors_, id);
        DCHECK(processor);
        processor->Message(message_timestamp, message_data);
      });
    }
    controller.messages.Clear(messages);
  }

  last_timestamp_ = timestamp;
  position_ = end_position;
}

}  // namespace barelyapi
