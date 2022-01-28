#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each `Process` call.
constexpr int kNumMaxTasks = 100;

}  // namespace

InstrumentManager::InstrumentManager() noexcept : runner_(kNumMaxTasks) {}

Status InstrumentManager::Create(Id instrument_id,
                                 InstrumentDefinition definition,
                                 int sample_rate) noexcept {
  if (instrument_id == kInvalidId) return Status::kInvalidArgument;
  if (controllers_.emplace(instrument_id, InstrumentController(definition))
          .second) {
    runner_.Add([this, instrument_id, definition = std::move(definition),
                 sample_rate]() noexcept {
      processors_.emplace(
          std::piecewise_construct, std::forward_as_tuple(instrument_id),
          std::forward_as_tuple(std::move(definition), sample_rate));
    });
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

Status InstrumentManager::Destroy(Id instrument_id) noexcept {
  if (controllers_.erase(instrument_id) > 0) {
    runner_.Add(
        [this, instrument_id]() noexcept { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> InstrumentManager::GetGain(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetGain();
  }
  return Status::kNotFound;
}

StatusOr<Param> InstrumentManager::GetParam(Id instrument_id,
                                            int param_index) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto* param = controller->GetParam(param_index)) {
      return *param;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsMuted(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsMuted();
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsNoteOn(Id instrument_id,
                                           float note_pitch) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return Status::kNotFound;
}

void InstrumentManager::Process(Id instrument_id, double timestamp,
                                float* output, int num_channels,
                                int num_frames) noexcept {
  runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(output, num_channels, num_frames, timestamp);
  } else {
    std::fill_n(output, num_channels * num_frames, 0.0f);
  }
}

void InstrumentManager::ProcessEvent(Id instrument_id, double timestamp,
                                     InstrumentEvent event) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ProcessEvent(std::move(event), timestamp);
  }
}

void InstrumentManager::SetAllNotesOff(double timestamp) noexcept {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.StopAllNotes(timestamp);
  }
}

Status InstrumentManager::SetAllNotesOff(Id instrument_id,
                                         double timestamp) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StopAllNotes(timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetAllParamsToDefault(double timestamp) noexcept {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.ResetAllParams(timestamp);
  }
}

Status InstrumentManager::SetAllParamsToDefault(Id instrument_id,
                                                double timestamp) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParams(timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetData(Id instrument_id, double timestamp,
                                  void* data) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetData(data, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetGain(Id instrument_id, double timestamp,
                                  float gain) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetGain(gain, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetMuted(Id instrument_id, double timestamp,
                                   bool is_muted) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetMuted(is_muted, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOff(Id instrument_id, double timestamp,
                                     float note_pitch) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StopNote(note_pitch, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOffCallback(
    Id instrument_id,
    InstrumentController::NoteOffCallback note_off_callback) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOffCallback(std::move(note_off_callback));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOn(Id instrument_id, double timestamp,
                                    float note_pitch,
                                    float note_intensity) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StartNote(note_pitch, note_intensity, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOnCallback(
    Id instrument_id,
    InstrumentController::NoteOnCallback note_on_callback) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOnCallback(std::move(note_on_callback));
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetParam(Id instrument_id, double timestamp,
                                   int param_index,
                                   float param_value) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(param_index, param_value, timestamp)) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetParamToDefault(Id instrument_id, double timestamp,
                                            int param_index) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(param_index, timestamp)) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::Update() noexcept {
  // TODO: This is a temp hack to keep update working until controller is
  // refactored back to using external update events map.
  for (auto& [instrument_id, controller] : controllers_) {
    auto events = controller.ExtractEvents();
    if (!events.empty()) {
      update_events_[instrument_id].merge(std::move(events));
    }
  }
  if (!update_events_.empty()) {
    runner_.Add(
        [this, update_events = std::exchange(update_events_, {})]() noexcept {
          for (auto& [instrument_id, events] : update_events) {
            if (auto* processor = FindOrNull(processors_, instrument_id)) {
              processor->MergeEvents(std::move(events));
            }
          }
        });
  }
}

}  // namespace barelyapi
