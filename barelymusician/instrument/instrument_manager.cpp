#include "barelymusician/instrument/instrument_manager.h"

#include <algorithm>
#include <utility>

#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each |Process| call.
constexpr int kNumMaxTasks = 1000;

}  // namespace

InstrumentManager::InstrumentManager()
    : id_counter_(0), task_runner_(kNumMaxTasks) {}

StatusOr<int64> InstrumentManager::Create(
    InstrumentDefinition definition,
    InstrumentParamDefinitions param_definitions) {
  const int64 instrument_id = ++id_counter_;
  InstrumentController controller(param_definitions);
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = controller.GetAllParams()]() {
    InstrumentProcessor processor(std::move(definition));
    for (const auto& [id, value] : params) {
      processor.SetData(0, Param{id, value});
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
  return instrument_id;
}

Status InstrumentManager::Destroy(int64 instrument_id) {
  if (controllers_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::vector<float>> InstrumentManager::GetAllNotes(
    int64 instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return Status::kNotFound;
}

StatusOr<std::vector<Param>> InstrumentManager::GetAllParams(
    int64 instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return Status::kNotFound;
}

StatusOr<float> InstrumentManager::GetParam(int64 instrument_id,
                                            int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const float* value = controller->GetParam(param_id)) {
      return *value;
    }
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsNoteOn(int64 instrument_id,
                                           float pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(pitch);
  }
  return Status::kNotFound;
}

Status InstrumentManager::Process(int64 instrument_id, int64 timestamp,
                                  float* output, int num_channels,
                                  int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::ResetParam(int64 instrument_id, int64 timestamp,
                                     int param_id) {
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

Status InstrumentManager::SetCustomData(int64 instrument_id, int64 timestamp,
                                        void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    SetProcessorData(instrument_id, timestamp, CustomData{custom_data});
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOff(int64 instrument_id, int64 timestamp,
                                     float pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOff(pitch)) {
      SetProcessorData(instrument_id, timestamp, NoteOff{pitch});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOn(int64 instrument_id, int64 timestamp,
                                    float pitch, float intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOn(pitch)) {
      SetProcessorData(instrument_id, timestamp, NoteOn{pitch, intensity});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetParam(int64 instrument_id, int64 timestamp,
                                   int param_id, float param_value) {
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

void InstrumentManager::SetProcessorData(int64 instrument_id, int64 timestamp,
                                         InstrumentData data) {
  task_runner_.Add([this, instrument_id, timestamp, data = std::move(data)]() {
    if (auto* processor = FindOrNull(processors_, instrument_id)) {
      processor->SetData(timestamp, std::move(data));
    }
  });
}

}  // namespace barelyapi
