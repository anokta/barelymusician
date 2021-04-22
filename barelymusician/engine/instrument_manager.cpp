#include "barelymusician/engine/instrument_manager.h"

#include <any>
#include <utility>
#include <vector>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 8000;

}  // namespace

InstrumentManager::InstrumentManager(int sample_rate, IdGenerator* id_generator)
    : sample_rate_(sample_rate),
      id_generator_(id_generator),
      task_runner_(kNumMaxTasks),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  DCHECK_GE(sample_rate, 0);
  DCHECK(id_generator);
}

Id InstrumentManager::Create(InstrumentDefinition definition,
                             InstrumentParamDefinitions param_definitions) {
  const Id instrument_id = id_generator_->Generate();
  InstrumentController controller(
      param_definitions,
      [&](float pitch) {
        if (note_off_callback_) {
          note_off_callback_(instrument_id, pitch);
        }
      },
      [&](float pitch, float intensity) {
        if (note_on_callback_) {
          note_on_callback_(instrument_id, pitch, intensity);
        }
      });

  InstrumentProcessorEvents param_events;
  for (const auto& param : controller.GetAllParams()) {
    param_events.emplace(0.0, barelyapi::SetParam{param.id, param.value});
  }
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = std::move(param_events)]() {
    InstrumentProcessor processor(sample_rate_, std::move(definition));
    processor.Schedule((std::move(params)));
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
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

std::vector<float> InstrumentManager::GetAllNotes(Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return {};
}

std::vector<InstrumentParam> InstrumentManager::GetAllParams(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return {};
}

const float* InstrumentManager::GetParam(Id instrument_id, int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetParam(param_id);
  }
  return nullptr;
}

bool InstrumentManager::IsNoteOn(Id instrument_id, float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return false;
}

bool InstrumentManager::Process(Id instrument_id, float* output,
                                int num_channels, int num_frames,
                                double timestamp) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return true;
  }
  return false;
}

void InstrumentManager::ResetAllParams(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.Schedule(timestamp, barelyapi::ResetAllParams{});
  }
}

bool InstrumentManager::ResetAllParams(Id instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, barelyapi::ResetAllParams{});
    return true;
  }
  return false;
}

bool InstrumentManager::ResetParam(Id instrument_id, int param_id,
                                   double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, barelyapi::ResetParam{param_id});
    return true;
  }
  return false;
}

void InstrumentManager::SetAllNotesOff(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.Schedule(timestamp, barelyapi::SetAllNotesOff{});
  }
}

bool InstrumentManager::SetAllNotesOff(Id instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, barelyapi::SetAllNotesOff{});
    return true;
  }
  return false;
}

bool InstrumentManager::SetCustomData(Id instrument_id, std::any custom_data,
                                      double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp,
                         barelyapi::SetCustomData{std::move(custom_data)});
    return true;
  }
  return false;
}

bool InstrumentManager::SetEvents(Id instrument_id,
                                  InstrumentControllerEvents events) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(std::move(events));
    return true;
  }
  return false;
}

bool InstrumentManager::SetNoteOff(Id instrument_id, float note_pitch,
                                   double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, barelyapi::SetNoteOff{note_pitch});
    return true;
  }
  return false;
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

bool InstrumentManager::SetNoteOn(Id instrument_id, float note_pitch,
                                  float note_intensity, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp,
                         barelyapi::SetNoteOn{note_pitch, note_intensity});
    return true;
  }
  return false;
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

bool InstrumentManager::SetParam(Id instrument_id, int param_id,
                                 float param_value, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, barelyapi::SetParam{param_id, param_value});
    return true;
  }
  return false;
}

void InstrumentManager::SetSampleRate(int sample_rate) {
  SetAllNotesOff();
  task_runner_.Add([this, sample_rate]() {
    sample_rate_ = sample_rate;
    for (auto& [instrument_id, processor] : processors_) {
      processor.Reset(sample_rate);
    }
  });
}

void InstrumentManager::Update(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    task_runner_.Add([this, instrument_id = instrument_id,
                      events = controller.Update(timestamp)]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        processor->Schedule(std::move(events));
      }
    });
  }
}

}  // namespace barelyapi
