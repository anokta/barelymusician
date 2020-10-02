#include "barelymusician/engine/instrument_manager.h"

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine_utils.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each Process call.
constexpr int kNumMaxTasks = 500;

}  // namespace

InstrumentManager::InstrumentManager()
    : note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {}

int InstrumentManager::Create(InstrumentDefinition definition) {
  const int instrument_id = ++id_counter_;
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

bool InstrumentManager::Destroy(int instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return true;
  }
  return false;
}

std::optional<float> InstrumentManager::GetParam(int instrument_id,
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

std::optional<bool> InstrumentManager::IsNoteOn(int instrument_id,
                                                float index) const {
  const auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    return controller->active_notes.find(index) !=
           controller->active_notes.cend();
  }
  return std::nullopt;
}

bool InstrumentManager::AllNotesOff(int instrument_id) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller == nullptr) {
    return false;
  }
  controller->messages.Clear();
  task_runner_.Add([this, instrument_id, notes = controller->active_notes]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    DCHECK(processor);
    processor->messages.Clear();
    for (const auto& note : notes) {
      processor->instrument->NoteOff(note);
    }
  });
  if (note_off_callback_ != nullptr) {
    for (const auto& note : controller->active_notes) {
      note_off_callback_(instrument_id, note);
    }
  }
  controller->active_notes.clear();
  return true;
}

bool InstrumentManager::Control(int instrument_id, int id, float value) {
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

bool InstrumentManager::NoteOff(int instrument_id, float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.erase(index);
    if (note_off_callback_ != nullptr) {
      note_off_callback_(instrument_id, index);
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

bool InstrumentManager::NoteOn(int instrument_id, float index,
                               float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.insert(index);
    if (note_on_callback_ != nullptr) {
      note_on_callback_(instrument_id, index, intensity);
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

bool InstrumentManager::Process(int instrument_id, double begin_timestamp,
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

bool InstrumentManager::ResetAllParams(int instrument_id) {
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

bool InstrumentManager::ScheduleControl(int instrument_id, double timestamp,
                                        int id, float value) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller == nullptr) {
    return false;
  }
  auto* param = FindOrNull(controller->params, id);
  if (param == nullptr) {
    return false;
  }
  controller->messages.Push(timestamp, ControlData{id, value});
  task_runner_.Add([this, instrument_id, timestamp, id, value]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    DCHECK(processor);
    processor->messages.Push(timestamp, ControlData{id, value});
  });
  return true;
}

bool InstrumentManager::ScheduleNote(int instrument_id, double timestamp,
                                     double duration, float index,
                                     float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(timestamp, NoteOnData{index, intensity});
    controller->messages.Push(timestamp + duration, NoteOffData{index});
    task_runner_.Add(
        [this, instrument_id, timestamp, duration, index, intensity]() {
          auto* processor = FindOrNull(processors_, instrument_id);
          DCHECK(processor);
          processor->messages.Push(timestamp, NoteOnData{index, intensity});
          processor->messages.Push(timestamp + duration, NoteOffData{index});
        });
    return true;
  }
  return false;
}

bool InstrumentManager::ScheduleNoteOff(int instrument_id, double timestamp,
                                        float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(timestamp, NoteOffData{index});
    task_runner_.Add([this, instrument_id, timestamp, index]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->messages.Push(timestamp, NoteOffData{index});
    });
    return true;
  }
  return false;
}

bool InstrumentManager::ScheduleNoteOn(int instrument_id, double timestamp,
                                       float index, float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(timestamp, NoteOnData{index, intensity});
    task_runner_.Add([this, instrument_id, timestamp, index, intensity]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      DCHECK(processor);
      processor->messages.Push(timestamp, NoteOnData{index, intensity});
    });
    return true;
  }
  return false;
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void InstrumentManager::Update(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    const auto messages = controller.messages.GetIterator(timestamp);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      std::visit(
          MessageVisitor{
              [this, instrument_id = instrument_id,
               controller = &controller](const ControlData& control_data) {
                controller->params.at(control_data.id) = control_data.value;
              },
              [this, instrument_id = instrument_id,
               controller = &controller](const NoteOffData& note_off_data) {
                controller->active_notes.erase(note_off_data.index);
                if (note_off_callback_ != nullptr) {
                  note_off_callback_(instrument_id, note_off_data.index);
                }
              },
              [this, instrument_id = instrument_id,
               controller = &controller](const NoteOnData& note_on_data) {
                controller->active_notes.insert(note_on_data.index);
                if (note_on_callback_ != nullptr) {
                  note_on_callback_(instrument_id, note_on_data.index,
                                    note_on_data.intensity);
                }
              }},
          it->data);
    }
    controller.messages.Clear(messages);
  }
}

}  // namespace barelyapi
