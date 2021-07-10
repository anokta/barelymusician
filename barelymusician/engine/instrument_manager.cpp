#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <unordered_map>
#include <utility>
#include <variant>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"
#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 100;

// Dummy note off callback that does nothing.
void NoopNoteOffCallback(Id /*instrument_id*/, double /*timestamp*/,
                         float /*note_pitch*/) {}

// Dummy note on callback that does nothing.
void NoopNoteOnCallback(Id /*instrument_id*/, double /*timestamp*/,
                        float /*note_pitch*/, float /*note_intensity*/) {}

}  // namespace

InstrumentManager::InstrumentManager(int sample_rate)
    : audio_runner_(kNumMaxTasks),
      note_off_callback_(&NoopNoteOffCallback),
      note_on_callback_(&NoopNoteOnCallback),
      sample_rate_(sample_rate) {}

Status InstrumentManager::Create(Id instrument_id, double timestamp,
                                 InstrumentDefinition definition,
                                 InstrumentParamDefinitions param_definitions) {
  if (const auto [controller_it, success] = controllers_.emplace(
          instrument_id, InstrumentController(std::move(param_definitions)));
      success) {
    auto& update_events = update_events_[instrument_id];
    update_events.emplace(timestamp, CreateEvent{std::move(definition)});
    for (const auto& [id, param] : controller_it->second.params) {
      update_events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
    }
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

Status InstrumentManager::Destroy(Id instrument_id, double timestamp) {
  if (const auto controller_it = controllers_.find(instrument_id);
      controller_it != controllers_.end()) {
    auto& update_events = update_events_[instrument_id];
    for (const float pitch : controller_it->second.pitches) {
      note_off_callback_(instrument_id, timestamp, pitch);
      update_events.emplace(timestamp, SetNoteOffEvent{pitch});
    }
    update_events.emplace(timestamp, DestroyEvent{});
    controllers_.erase(controller_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::vector<float>> InstrumentManager::GetAllNotes(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return std::vector<float>(controller->pitches.cbegin(),
                              controller->pitches.cend());
  }
  return Status::kNotFound;
}

StatusOr<std::vector<InstrumentParam>> InstrumentManager::GetAllParams(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    std::vector<InstrumentParam> params;
    params.reserve(controller->params.size());
    for (const auto& [id, param] : controller->params) {
      params.push_back(param);
    }
    return params;
  }
  return Status::kNotFound;
}

StatusOr<InstrumentParam> InstrumentManager::GetParam(Id instrument_id,
                                                      int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto* param = FindOrNull(controller->params, param_id)) {
      return *param;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsNoteOn(Id instrument_id,
                                           float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->pitches.find(note_pitch) != controller->pitches.cend();
  }
  return Status::kNotFound;
}

void InstrumentManager::Process(Id instrument_id, double timestamp,
                                float* output, int num_channels,
                                int num_frames) {
  audio_runner_.Run();
  if (const auto processor_it = processors_.find(instrument_id);
      processor_it != processors_.end()) {
    int frame = 0;
    auto& instrument = processor_it->second.instrument;
    auto process_until_fn = [&](int end_frame) {
      auto* process_output = &output[num_channels * frame];
      if (instrument) {
        instrument->Process(process_output, num_channels, end_frame - frame);
      } else {
        std::fill_n(process_output, num_channels * (end_frame - frame), 0.0f);
      }
    };
    // Process *all* events before the end timestamp.
    auto& events = processor_it->second.events;
    auto begin = events.begin();
    auto end = events.lower_bound(timestamp +
                                  SecondsFromSamples(sample_rate_, num_frames));
    for (auto it = begin; it != end; ++it) {
      const int message_frame =
          SamplesFromSeconds(sample_rate_, it->first - timestamp);
      if (frame < message_frame) {
        process_until_fn(message_frame);
        frame = message_frame;
      }
      std::visit(
          InstrumentEventVisitor{
              [&](CreateEvent& create_event) {
                instrument.emplace(sample_rate_,
                                   std::move(create_event.definition));
              },
              [&](DestroyEvent& /*destroy_event*/) { instrument.reset(); },
              [&](SetCustomDataEvent& set_custom_data_event) {
                if (instrument) {
                  instrument->SetCustomData(
                      std::move(set_custom_data_event.data));
                }
              },
              [&](SetNoteOffEvent& set_note_off_event) {
                if (instrument) {
                  instrument->SetNoteOff(set_note_off_event.pitch);
                }
              },
              [&](SetNoteOnEvent& set_note_on_event) {
                if (instrument) {
                  instrument->SetNoteOn(set_note_on_event.pitch,
                                        set_note_on_event.intensity);
                }
              },
              [&](SetParamEvent& set_param_event) {
                if (instrument) {
                  instrument->SetParam(set_param_event.id,
                                       set_param_event.value);
                }
              }},
          it->second);
    }
    // Process the rest of the buffer.
    if (frame < num_frames) {
      process_until_fn(num_frames);
    }
    // Clean up the processor.
    events.erase(begin, end);
    if (!instrument && events.empty()) {
      processors_.erase(processor_it);
    }
  } else {
    std::fill_n(output, num_channels * num_frames, 0.0f);
  }
}

void InstrumentManager::SetAllNotesOff(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    auto& update_events = update_events_[instrument_id];
    for (const float pitch : controller.pitches) {
      note_off_callback_(instrument_id, timestamp, pitch);
      update_events.emplace(timestamp, SetNoteOffEvent{pitch});
    }
    controller.pitches.clear();
  }
}

Status InstrumentManager::SetAllNotesOff(Id instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto& update_events = update_events_[instrument_id];
    for (const float pitch : controller->pitches) {
      note_off_callback_(instrument_id, timestamp, pitch);
      update_events.emplace(timestamp, SetNoteOffEvent{pitch});
    }
    controller->pitches.clear();
    return Status::kOk;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetAllParamsToDefault(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    auto& update_events = update_events_[instrument_id];
    for (auto& [id, param] : controller.params) {
      param.ResetValue();
      update_events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
    }
  }
}

Status InstrumentManager::SetAllParamsToDefault(Id instrument_id,
                                                double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto& update_events = update_events_[instrument_id];
    for (auto& [id, param] : controller->params) {
      param.ResetValue();
      update_events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetCustomData(Id instrument_id, double timestamp,
                                        std::any custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    update_events_[instrument_id].emplace(
        timestamp, SetCustomDataEvent{std::move(custom_data)});
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOff(Id instrument_id, double timestamp,
                                     float note_pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->pitches.erase(note_pitch) > 0) {
      note_off_callback_(instrument_id, timestamp, note_pitch);
      update_events_[instrument_id].emplace(timestamp,
                                            SetNoteOffEvent{note_pitch});
      return Status::kOk;
    }
    return Status::kFailedPrecondition;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ =
      note_off_callback ? std::move(note_off_callback) : &NoopNoteOffCallback;
}

Status InstrumentManager::SetNoteOn(Id instrument_id, double timestamp,
                                    float note_pitch, float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->pitches.emplace(note_pitch).second) {
      note_on_callback_(instrument_id, timestamp, note_pitch, note_intensity);
      update_events_[instrument_id].emplace(
          timestamp, SetNoteOnEvent{note_pitch, note_intensity});
      return Status::kOk;
    }
    return Status::kFailedPrecondition;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ =
      note_on_callback ? std::move(note_on_callback) : &NoopNoteOnCallback;
}

Status InstrumentManager::SetParam(Id instrument_id, double timestamp,
                                   int param_id, float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (auto* param = FindOrNull(controller->params, param_id)) {
      param->SetValue(param_value);
      update_events_[instrument_id].emplace(
          timestamp, SetParamEvent{param_id, param->GetValue()});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetParamToDefault(Id instrument_id, double timestamp,
                                            int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (auto* param = FindOrNull(controller->params, param_id)) {
      param->ResetValue();
      update_events_[instrument_id].emplace(
          timestamp, SetParamEvent{param_id, param->GetValue()});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::Update() {
  if (!update_events_.empty()) {
    audio_runner_.Add(
        [this, update_events = std::exchange(update_events_, {})]() mutable {
          for (auto& [instrument_id, events] : update_events) {
            processors_[instrument_id].events.merge(std::move(events));
          }
        });
  }
}

InstrumentManager::InstrumentController::InstrumentController(
    InstrumentParamDefinitions param_definitions) {
  params.reserve(param_definitions.size());
  for (auto& param_definition : param_definitions) {
    params.emplace(param_definition.id,
                   InstrumentParam(std::move(param_definition)));
  }
}

}  // namespace barelyapi
