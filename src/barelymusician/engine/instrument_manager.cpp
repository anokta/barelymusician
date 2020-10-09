#include "barelymusician/engine/instrument_manager.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine_utils.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each Process call.
constexpr int kNumMaxTasks = 500;

double BeatsFromSeconds(double tempo, double seconds) {
  return tempo * seconds / kSecondsFromMinutes;
}

double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

}  // namespace

InstrumentManager::InstrumentManager()
    : is_playing_(false),
      position_(0.0),
      tempo_(0.0),
      start_timestamp_(0.0),
      last_timestamp_(0.0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {}

InstrumentManager::Id InstrumentManager::Create(
    InstrumentDefinition definition) {
  const Id instrument_id = ++id_counter_;
  const auto it =
      controllers_.emplace(instrument_id, InstrumentController{definition});
  auto& controller = it.first->second;
  for (const auto& param : controller.definition.param_definitions) {
    controller.params.emplace(param.id, param.default_value);
  }
  auto instrument = definition.get_instrument_fn();
  task_runner_.Add([this, instrument_id,
                    param_definitions = definition.param_definitions,
                    instrument = std::make_shared<std::unique_ptr<Instrument>>(
                        std::move(instrument))]() {
    for (const auto& param : param_definitions) {
      (*instrument)->Control(param.id, param.default_value);
    }
    processors_.emplace(instrument_id,
                        InstrumentProcessor{std::move(*instrument)});
  });
  return instrument_id;
}

bool InstrumentManager::Destroy(Id instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return true;
  }
  return false;
}

double InstrumentManager::GetPosition() const { return position_; }

double InstrumentManager::GetTempo() const { return tempo_; }

bool InstrumentManager::IsPlaying() const { return is_playing_; }

std::optional<float> InstrumentManager::GetParam(Id instrument_id,
                                                 int id) const {
  const auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    const auto* param = FindOrNull(controller->params, id);
    if (param != nullptr) {
      return *param;
    }
  }
  return std::nullopt;
}

std::optional<bool> InstrumentManager::IsNoteOn(Id instrument_id,
                                                float index) const {
  const auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    return controller->active_notes.find(index) !=
           controller->active_notes.cend();
  }
  return std::nullopt;
}

bool InstrumentManager::AllNotesOff(Id instrument_id) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller == nullptr) {
    return false;
  }
  for (const auto& note : controller->active_notes) {
    task_runner_.Add([this, instrument_id, note = note]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->messages.Clear();
      processor->instrument->NoteOff(note);
    });
  }
  if (note_off_callback_ != nullptr) {
    for (const auto& note : controller->active_notes) {
      note_off_callback_(last_timestamp_, instrument_id, note);
    }
  }
  controller->active_notes.clear();
  return true;
}

bool InstrumentManager::Control(Id instrument_id, int id, float value) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    auto* param = FindOrNull(controller->params, id);
    if (param != nullptr) {
      *param = value;
      task_runner_.Add([this, instrument_id, id, value]() {
        auto* processor = FindOrNull(processors_, instrument_id);
        DCHECK(processor);
        processor->instrument->Control(id, value);
      });
      return true;
    }
  }
  return false;
}

bool InstrumentManager::NoteOff(Id instrument_id, float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.erase(index);
    if (note_off_callback_ != nullptr) {
      note_off_callback_(last_timestamp_, instrument_id, index);
    }
    task_runner_.Add([this, instrument_id, index]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->instrument->NoteOff(index);
    });
    return true;
  }
  return false;
}

bool InstrumentManager::NoteOn(Id instrument_id, float index, float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.insert(index);
    if (note_on_callback_ != nullptr) {
      note_on_callback_(last_timestamp_, instrument_id, index, intensity);
    }
    task_runner_.Add([this, instrument_id, index, intensity]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->instrument->NoteOn(index, intensity);
    });
    return true;
  }
  return false;
}

bool InstrumentManager::Process(Id instrument_id, double begin_timestamp,
                                double end_timestamp, float* output,
                                int num_channels, int num_frames) {
  task_runner_.Run();
  auto* processor = FindOrNull(processors_, instrument_id);
  if (processor == nullptr) {
    return false;
  }
  Instrument* instrument = processor->instrument.get();
  int frame = 0;
  // Process messages.
  if (begin_timestamp < end_timestamp) {
    // Include *all* messages before |end_timestamp|.
    const auto messages = processor->messages.GetIterator(end_timestamp);
    const double frame_rate =
        static_cast<double>(num_frames) / (end_timestamp - begin_timestamp);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      int message_frame =
          static_cast<int>(frame_rate * (it->timestamp - begin_timestamp));
      if (frame < message_frame) {
        instrument->Process(&output[num_channels * frame], num_channels,
                            message_frame - frame);
        frame = message_frame;
      }
      std::visit(MessageVisitor{[instrument](const ControlData& control_data) {
                                  instrument->Control(control_data.id,
                                                      control_data.value);
                                },
                                [instrument](const NoteOffData& note_off_data) {
                                  instrument->NoteOff(note_off_data.index);
                                },
                                [instrument](const NoteOnData& note_on_data) {
                                  instrument->NoteOn(note_on_data.index,
                                                     note_on_data.intensity);
                                }},
                 it->data);
    }
    processor->messages.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument->Process(&output[num_channels * frame], num_channels,
                        num_frames - frame);
  }
  return true;
}

bool InstrumentManager::ResetAllParams(Id instrument_id) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    for (const auto& param : controller->definition.param_definitions) {
      controller->params.at(param.id) = param.default_value;
    }
    task_runner_.Add(
        [this, instrument_id,
         param_definitions = controller->definition.param_definitions]() {
          auto* processor = FindOrNull(processors_, instrument_id);
          DCHECK(processor);
          for (const auto& param : param_definitions) {
            processor->instrument->Control(param.id, param.default_value);
          }
        });
    return true;
  }
  return false;
}

bool InstrumentManager::ScheduleControl(Id instrument_id, double position,
                                        int id, float value) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller == nullptr) {
    return false;
  }
  auto* param = FindOrNull(controller->params, id);
  if (param == nullptr) {
    return false;
  }
  controller->messages.Push(position, ControlData{id, value});
  return true;
}

bool InstrumentManager::ScheduleNote(Id instrument_id, double position,
                                     double duration, float index,
                                     float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(position, NoteOnData{index, intensity});
    controller->messages.Push(position + duration, NoteOffData{index});
    return true;
  }
  return false;
}

bool InstrumentManager::ScheduleNoteOff(Id instrument_id, double position,
                                        float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(position, NoteOffData{index});
    return true;
  }
  return false;
}

bool InstrumentManager::ScheduleNoteOn(Id instrument_id, double position,
                                       float index, float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(position, NoteOnData{index, intensity});
    return true;
  }
  return false;
}

void InstrumentManager::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void InstrumentManager::SetPosition(double position) { position_ = position; }

void InstrumentManager::SetTempo(double tempo) { tempo_ = tempo; }

void InstrumentManager::Start(double timestamp) {
  start_timestamp_ = timestamp;
  is_playing_ = true;
}

void InstrumentManager::Stop() {
  is_playing_ = false;
  for (const auto& [id, message] : controllers_) {
    AllNotesOff(id);
  }
}

void InstrumentManager::Update(double timestamp) {
  if (!is_playing_ || tempo_ == 0.0) {
    return;
  }
  const double elapsed_seconds =
      std::min(timestamp - last_timestamp_, timestamp - start_timestamp_);
  if (elapsed_seconds <= 0.0) {
    return;
  }

  const double elapsed_beats = BeatsFromSeconds(tempo_, elapsed_seconds);

  const double begin_timestamp =
      position_ > 0.0 ? last_timestamp_ : start_timestamp_;
  const double end_position = position_ + elapsed_beats;

  if (beat_callback_ != nullptr) {
    for (double beat = std::ceil(position_); beat < end_position; ++beat) {
      const double beat_timestamp =
          begin_timestamp + SecondsFromBeats(tempo_, beat - position_);
      beat_callback_(beat_timestamp, static_cast<int>(beat));
    }
  }

  for (auto& [id, controller] : controllers_) {
    const auto messages = controller.messages.GetIterator(end_position);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const double message_timestamp =
          begin_timestamp + SecondsFromBeats(tempo_, it->timestamp - position_);
      std::visit(
          MessageVisitor{
              [this, id = id, message_timestamp = message_timestamp,
               controller = &controller](const ControlData& control_data) {
                task_runner_.Add([this, id, message_timestamp, control_data]() {
                  auto* processor = FindOrNull(processors_, id);
                  DCHECK(processor);
                  processor->messages.Push(message_timestamp, control_data);
                });
                controller->params.at(control_data.id) = control_data.value;
              },
              [this, id = id, message_timestamp = message_timestamp,
               controller = &controller](const NoteOffData& note_off_data) {
                task_runner_.Add([this, id, message_timestamp,
                                  note_off_data]() {
                  auto* processor = FindOrNull(processors_, id);
                  DCHECK(processor);
                  processor->messages.Push(message_timestamp, note_off_data);
                });
                controller->active_notes.erase(note_off_data.index);
                if (note_off_callback_ != nullptr) {
                  note_off_callback_(message_timestamp, id,
                                     note_off_data.index);
                }
              },
              [this, id = id, message_timestamp = message_timestamp,
               controller = &controller](const NoteOnData& note_on_data) {
                task_runner_.Add([this, id, message_timestamp, note_on_data]() {
                  auto* processor = FindOrNull(processors_, id);
                  DCHECK(processor);
                  processor->messages.Push(message_timestamp, note_on_data);
                });
                controller->active_notes.insert(note_on_data.index);
                if (note_on_callback_ != nullptr) {
                  note_on_callback_(message_timestamp, id, note_on_data.index,
                                    note_on_data.intensity);
                }
              }},
          it->data);
    }
    controller.messages.Clear(messages);
  }
  last_timestamp_ = timestamp;
  position_ = end_position;
}

}  // namespace barelyapi
