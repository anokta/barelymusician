#include "barelymusician/engine/instrument_manager.h"

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine_utils.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each Process call.
constexpr int kNumMaxTasks = 500;

}  // namespace

InstrumentManager::InstrumentManager(int sample_rate)
    : note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      sample_rate_(sample_rate),
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
  auto instrument = definition.get_instrument_fn(sample_rate_);
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

void InstrumentManager::Destroy(int instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
  }
}

void InstrumentManager::GetParam(int instrument_id, int id,
                                 float* value) const {
  const auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    const auto* param = FindOrNull(controller->params, id);
    if (param != nullptr) {
      *value = *param;
    }
  }
}

void InstrumentManager::AllNotesOff() {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.messages.Clear();
    task_runner_.Add([this, instrument_id = instrument_id]() {
      auto* processor = FindOrNull(processors_, instrument_id);
      if (processor != nullptr) {
        processor->messages.Clear();
      }
    });
    auto active_notes_copy = controller.active_notes;
    for (auto& note : active_notes_copy) {
      NoteOff(instrument_id, note);
    }
  }
}

void InstrumentManager::IsNoteOn(int instrument_id, float index,
                                 bool* active) const {
  const auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    *active =
        controller->active_notes.find(index) != controller->active_notes.cend();
  }
}

void InstrumentManager::NoteOff(int instrument_id, float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.erase(index);
    if (note_off_callback_ != nullptr) {
      note_off_callback_(instrument_id, index);
    }
  }
  task_runner_.Add([this, instrument_id, index]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    if (processor != nullptr) {
      processor->instrument->NoteOff(index);
    }
  });
}

void InstrumentManager::NoteOn(int instrument_id, float index,
                               float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->active_notes.insert(index);
    if (note_on_callback_ != nullptr) {
      note_on_callback_(instrument_id, index, intensity);
    }
  }
  task_runner_.Add([this, instrument_id, index, intensity]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    if (processor != nullptr) {
      processor->instrument->NoteOn(index, intensity);
    }
  });
}

void InstrumentManager::Process(int instrument_id, double begin_timestamp,
                                double end_timestamp, float* output,
                                int num_channels, int num_frames) {
  task_runner_.Run();
  auto* processor = FindOrNull(processors_, instrument_id);
  if (processor == nullptr) {
    return;
  }
  Instrument* instrument = processor->instrument.get();
  int frame = 0;
  // Process messages.
  if (begin_timestamp < end_timestamp) {
    // Include *all* messages before |end_timestamp|.
    const auto messages = processor->messages.GetIterator(0.0, end_timestamp);
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
      std::visit(MessageVisitor{[instrument](const NoteOffData& note_off_data) {
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
}

void InstrumentManager::ScheduleNoteOff(int instrument_id, double timestamp,
                                        float index) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(timestamp, NoteOffData{index});
  }
  task_runner_.Add([this, instrument_id, timestamp, index]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    if (processor != nullptr) {
      processor->messages.Push(timestamp, NoteOffData{index});
    }
  });
}

void InstrumentManager::ScheduleNoteOn(int instrument_id, double timestamp,
                                       float index, float intensity) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    controller->messages.Push(timestamp, NoteOnData{index, intensity});
  }
  task_runner_.Add([this, instrument_id, timestamp, index, intensity]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    if (processor != nullptr) {
      processor->messages.Push(timestamp, NoteOnData{index, intensity});
    }
  });
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void InstrumentManager::SetParam(int instrument_id, int id, float value) {
  auto* controller = FindOrNull(controllers_, instrument_id);
  if (controller != nullptr) {
    auto* param = FindOrNull(controller->params, id);
    if (param != nullptr) {
      *param = value;
    }
  }
  task_runner_.Add([this, instrument_id, id, value]() {
    auto* processor = FindOrNull(processors_, instrument_id);
    if (processor != nullptr) {
      processor->instrument->Control(id, value);
    }
  });
}

void InstrumentManager::Update(double begin_timestamp, double end_timestamp) {
  if (begin_timestamp < end_timestamp) {
    for (auto& [instrument_id, controller] : controllers_) {
      // Include *all* messages before |end_timestamp|.
      const auto messages = controller.messages.GetIterator(0.0, end_timestamp);
      for (auto it = messages.cbegin; it != messages.cend; ++it) {
        std::visit(
            MessageVisitor{
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
}

}  // namespace barelyapi
