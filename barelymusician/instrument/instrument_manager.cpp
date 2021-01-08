#include "barelymusician/instrument/instrument_manager.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/id.h"
#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 1000;

}  // namespace

InstrumentManager::InstrumentManager()
    : note_off_callback_(nullptr),
      note_on_callback_(nullptr),
      task_runner_(kNumMaxTasks) {}

std::int64_t InstrumentManager::Create(
    InstrumentDefinition definition,
    InstrumentParamDefinitions param_definitions, std::int64_t timestamp) {
  const std::int64_t instrument_id = GetNextId();
  InstrumentController controller(param_definitions);
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = controller.GetAllParams(), timestamp]() {
    InstrumentProcessor processor(std::move(definition));
    for (const auto& [id, value] : params) {
      processor.SetData(timestamp, Param{id, value});
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
  return instrument_id;
}

Status InstrumentManager::Destroy(std::int64_t instrument_id,
                                  std::int64_t timestamp) {
  if (const auto it = controllers_.find(instrument_id);
      it != controllers_.end()) {
    if (note_off_callback_) {
      for (const auto& note_pitch : it->second.GetAllNotes()) {
        note_off_callback_(instrument_id, timestamp, note_pitch);
      }
    }
    controllers_.erase(it);
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::vector<float>> InstrumentManager::GetAllNotes(
    std::int64_t instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return Status::kNotFound;
}

StatusOr<std::vector<std::pair<int, float>>> InstrumentManager::GetAllParams(
    std::int64_t instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return Status::kNotFound;
}

StatusOr<float> InstrumentManager::GetParam(std::int64_t instrument_id,
                                            int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const float* value = controller->GetParam(param_id)) {
      return *value;
    }
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsNoteOn(std::int64_t instrument_id,
                                           float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return Status::kNotFound;
}

Status InstrumentManager::Process(std::int64_t instrument_id,
                                  std::int64_t timestamp, float* output,
                                  int num_channels, int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void InstrumentManager::ResetAllParams(std::int64_t timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.ResetAllParams();
    task_runner_.Add([this, instrument_id = instrument_id, timestamp,
                      params = controller.GetAllParams()]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& [id, value] : params) {
          processor->SetData(timestamp, Param{id, value});
        }
      }
    });
  }
}

Status InstrumentManager::ResetAllParams(std::int64_t instrument_id,
                                         std::int64_t timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParams();
    task_runner_.Add([this, instrument_id, timestamp,
                      params = controller->GetAllParams()]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& [id, value] : params) {
          processor->SetData(timestamp, Param{id, value});
        }
      }
    });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::ResetParam(std::int64_t instrument_id,
                                     std::int64_t timestamp, int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(param_id)) {
      SetProcessorData(instrument_id, timestamp,
                       Param{param_id, *controller->GetParam(param_id)});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetAllNotesOff(std::int64_t timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    auto notes = controller.GetAllNotes();
    controller.SetAllNotesOff();
    if (note_off_callback_) {
      for (const auto& note_pitch : notes) {
        note_off_callback_(instrument_id, timestamp, note_pitch);
      }
    }
    task_runner_.Add([this, instrument_id = instrument_id, timestamp,
                      notes = std::move(notes)]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& note_pitch : notes) {
          processor->SetData(timestamp, NoteOff{note_pitch});
        }
      }
    });
  }
}

Status InstrumentManager::SetAllNotesOff(std::int64_t instrument_id,
                                         std::int64_t timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto notes = controller->GetAllNotes();
    controller->SetAllNotesOff();
    if (note_off_callback_) {
      for (const auto& note_pitch : notes) {
        note_off_callback_(instrument_id, timestamp, note_pitch);
      }
    }
    task_runner_.Add(
        [this, instrument_id, timestamp, notes = std::move(notes)]() {
          if (auto* processor = FindOrNull(processors_, instrument_id)) {
            for (const auto& note_pitch : notes) {
              processor->SetData(timestamp, NoteOff{note_pitch});
            }
          }
        });
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetCustomData(std::int64_t instrument_id,
                                        std::int64_t timestamp,
                                        void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    SetProcessorData(instrument_id, timestamp, CustomData{custom_data});
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOff(std::int64_t instrument_id,
                                     std::int64_t timestamp, float note_pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOff(note_pitch)) {
      if (note_off_callback_) {
        note_off_callback_(instrument_id, timestamp, note_pitch);
      }
      SetProcessorData(instrument_id, timestamp, NoteOff{note_pitch});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetNoteOffCallback(
    InstrumentNoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

Status InstrumentManager::SetNoteOn(std::int64_t instrument_id,
                                    std::int64_t timestamp, float note_pitch,
                                    float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOn(note_pitch)) {
      if (note_on_callback_) {
        note_on_callback_(instrument_id, timestamp, note_pitch, note_intensity);
      }
      SetProcessorData(instrument_id, timestamp,
                       NoteOn{note_pitch, note_intensity});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetNoteOnCallback(
    InstrumentNoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

Status InstrumentManager::SetParam(std::int64_t instrument_id,
                                   std::int64_t timestamp, int param_id,
                                   float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(param_id, param_value)) {
      SetProcessorData(instrument_id, timestamp,
                       Param{param_id, *controller->GetParam(param_id)});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetProcessorData(std::int64_t instrument_id,
                                         std::int64_t timestamp,
                                         InstrumentData data) {
  task_runner_.Add([this, instrument_id, timestamp, data = std::move(data)]() {
    if (auto* processor = FindOrNull(processors_, instrument_id)) {
      processor->SetData(timestamp, std::move(data));
    }
  });
}

}  // namespace barelyapi
