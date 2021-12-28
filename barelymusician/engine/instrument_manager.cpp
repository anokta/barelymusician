#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <utility>
#include <variant>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/common/visitor.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"

namespace barely {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 100;

// Dummy note off callback that does nothing.
void NoopNoteOffCallback(Id /*instrument_id*/, double /*timestamp*/,
                         float /*note_pitch*/) noexcept {}

// Dummy note on callback that does nothing.
void NoopNoteOnCallback(Id /*instrument_id*/, double /*timestamp*/,
                        float /*note_pitch*/,
                        float /*note_intensity*/) noexcept {}

}  // namespace

InstrumentManager::InstrumentManager(int sample_rate) noexcept
    : audio_runner_(kNumMaxTasks),
      note_off_callback_(&NoopNoteOffCallback),
      note_on_callback_(&NoopNoteOnCallback),
      sample_rate_(sample_rate) {}

Status InstrumentManager::Add(Id instrument_id, double timestamp,
                              InstrumentDefinition definition) noexcept {
  if (instrument_id == kInvalidId) {
    return Status::kInvalidArgument;
  }
  if (const auto [controller_it, success] =
          controllers_.emplace(instrument_id, InstrumentController(definition));
      success) {
    auto& update_events = update_events_[instrument_id];
    update_events.emplace(timestamp, CreateEvent{std::move(definition)});
    const auto& params = controller_it->second.params;
    for (int i = 0; i < static_cast<int>(params.size()); ++i) {
      update_events.emplace(timestamp, SetParamEvent{i, params[i].GetValue()});
    }
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

StatusOr<float> InstrumentManager::GetGain(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->gain;
  }
  return Status::kNotFound;
}

StatusOr<Param> InstrumentManager::GetParam(Id instrument_id,
                                            int param_index) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (param_index >= 0 &&
        param_index < static_cast<int>(controller->params.size())) {
      return controller->params[param_index];
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsMuted(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->is_muted;
  }
  return Status::kNotFound;
}

StatusOr<bool> InstrumentManager::IsNoteOn(Id instrument_id,
                                           float note_pitch) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->pitches.find(note_pitch) != controller->pitches.cend();
  }
  return Status::kNotFound;
}

bool InstrumentManager::IsValid(Id instrument_id) const noexcept {
  return controllers_.contains(instrument_id);
}

void InstrumentManager::Process(Id instrument_id, double timestamp,
                                float* output, int num_channels,
                                int num_frames) noexcept {
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
    const int sample_rate = sample_rate_;
    auto& events = processor_it->second.events;
    const auto begin = events.begin();
    const auto end = events.lower_bound(
        timestamp + SecondsFromSamples(sample_rate, num_frames));
    for (auto it = begin; it != end; ++it) {
      const int message_frame =
          SamplesFromSeconds(sample_rate, it->first - timestamp);
      if (frame < message_frame) {
        process_until_fn(message_frame);
        frame = message_frame;
      }
      std::visit(
          Visitor{[&](CreateEvent& create_event) noexcept {
                    instrument.emplace(sample_rate,
                                       std::move(create_event.definition));
                  },
                  [&](DestroyEvent& /*destroy_event*/) noexcept {
                    instrument.reset();
                  },
                  [&](SetCustomDataEvent& set_custom_data_event) noexcept {
                    if (instrument) {
                      instrument->SetCustomData(
                          std::move(set_custom_data_event.data));
                    }
                  },
                  [&](SetGainEvent& set_gain_event) noexcept {
                    if (instrument) {
                      instrument->SetGain(set_gain_event.gain);
                    }
                  },
                  [&](SetNoteOffEvent& set_note_off_event) noexcept {
                    if (instrument) {
                      instrument->SetNoteOff(set_note_off_event.pitch);
                    }
                  },
                  [&](SetNoteOnEvent& set_note_on_event) noexcept {
                    if (instrument) {
                      instrument->SetNoteOn(set_note_on_event.pitch,
                                            set_note_on_event.intensity);
                    }
                  },
                  [&](SetParamEvent& set_param_event) noexcept {
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

void InstrumentManager::ProcessEvent(Id instrument_id, double timestamp,
                                     InstrumentEvent event) noexcept {
  std::visit(
      Visitor{[&](SetAllNotesOffEvent& /*set_all_notes_off_event*/) noexcept {
                SetAllNotesOff(instrument_id, timestamp);
              },
              [&](SetAllParamsToDefaultEvent&
                  /*set_all_params_to_default_event*/) noexcept {
                SetAllParamsToDefault(instrument_id, timestamp);
              },
              [&](SetCustomDataEvent& set_custom_data_event) noexcept {
                SetCustomData(instrument_id, timestamp,
                              std::move(set_custom_data_event.data));
              },
              [&](SetGainEvent& set_gain_event) noexcept {
                SetGain(instrument_id, timestamp, set_gain_event.gain);
              },
              [&](SetNoteOffEvent& set_note_off_event) noexcept {
                SetNoteOff(instrument_id, timestamp, set_note_off_event.pitch);
              },
              [&](SetNoteOnEvent& set_note_on_event) noexcept {
                SetNoteOn(instrument_id, timestamp, set_note_on_event.pitch,
                          set_note_on_event.intensity);
              },
              [&](SetParamEvent& set_param_event) noexcept {
                SetParam(instrument_id, timestamp, set_param_event.id,
                         set_param_event.value);
              },
              [&](SetParamToDefaultEvent& set_param_to_default_event) noexcept {
                SetParamToDefault(instrument_id, timestamp,
                                  set_param_to_default_event.id);
              }},
      event);
}

Status InstrumentManager::Remove(Id instrument_id, double timestamp) noexcept {
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

void InstrumentManager::SetAllNotesOff(double timestamp) noexcept {
  for (auto& [instrument_id, controller] : controllers_) {
    if (!controller.pitches.empty()) {
      auto& update_events = update_events_[instrument_id];
      for (const float pitch : controller.pitches) {
        note_off_callback_(instrument_id, timestamp, pitch);
        update_events.emplace(timestamp, SetNoteOffEvent{pitch});
      }
      controller.pitches.clear();
    }
  }
}

Status InstrumentManager::SetAllNotesOff(Id instrument_id,
                                         double timestamp) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->pitches.empty()) {
      auto& update_events = update_events_[instrument_id];
      for (const float pitch : controller->pitches) {
        note_off_callback_(instrument_id, timestamp, pitch);
        update_events.emplace(timestamp, SetNoteOffEvent{pitch});
      }
      controller->pitches.clear();
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetAllParamsToDefault(double timestamp) noexcept {
  for (auto& [instrument_id, controller] : controllers_) {
    if (!controller.params.empty()) {
      auto& update_events = update_events_[instrument_id];
      for (int i = 0; i < static_cast<int>(controller.params.size()); ++i) {
        controller.params[i].ResetValue();
        update_events.emplace(
            timestamp, SetParamEvent{i, controller.params[i].GetValue()});
      }
    }
  }
}

Status InstrumentManager::SetAllParamsToDefault(Id instrument_id,
                                                double timestamp) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->params.empty()) {
      auto& update_events = update_events_[instrument_id];
      for (int i = 0; i < static_cast<int>(controller->params.size()); ++i) {
        controller->params[i].ResetValue();
        update_events.emplace(
            timestamp, SetParamEvent{i, controller->params[i].GetValue()});
      }
      return Status::kOk;
    }
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetCustomData(Id instrument_id, double timestamp,
                                        std::any custom_data) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    update_events_[instrument_id].emplace(
        timestamp, SetCustomDataEvent{std::move(custom_data)});
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetGain(Id instrument_id, double timestamp,
                                  float gain) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->gain = std::min(std::max(gain, 0.0f), 1.0f);
    if (!controller->is_muted) {
      update_events_[instrument_id].emplace(timestamp,
                                            SetGainEvent{controller->gain});
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetMuted(Id instrument_id, double timestamp,
                                   bool is_muted) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->is_muted != is_muted) {
      controller->is_muted = is_muted;
      update_events_[instrument_id].emplace(
          timestamp, SetGainEvent{is_muted ? 0.0f : controller->gain});
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetNoteOff(Id instrument_id, double timestamp,
                                     float note_pitch) noexcept {
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

void InstrumentManager::SetNoteOffCallback(
    NoteOffCallback note_off_callback) noexcept {
  note_off_callback_ =
      note_off_callback ? std::move(note_off_callback) : &NoopNoteOffCallback;
}

Status InstrumentManager::SetNoteOn(Id instrument_id, double timestamp,
                                    float note_pitch,
                                    float note_intensity) noexcept {
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

void InstrumentManager::SetNoteOnCallback(
    NoteOnCallback note_on_callback) noexcept {
  note_on_callback_ =
      note_on_callback ? std::move(note_on_callback) : &NoopNoteOnCallback;
}

Status InstrumentManager::SetParam(Id instrument_id, double timestamp,
                                   int param_index,
                                   float param_value) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (param_index >= 0 &&
        param_index < static_cast<int>(controller->params.size())) {
      auto& param = controller->params[param_index];
      if (param.SetValue(param_value)) {
        update_events_[instrument_id].emplace(
            timestamp, SetParamEvent{param_index, param.GetValue()});
      }
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status InstrumentManager::SetParamToDefault(Id instrument_id, double timestamp,
                                            int param_index) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (param_index >= 0 &&
        param_index < static_cast<int>(controller->params.size())) {
      auto& param = controller->params[param_index];
      if (param.ResetValue()) {
        update_events_[instrument_id].emplace(
            timestamp, SetParamEvent{param_index, param.GetValue()});
      }
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void InstrumentManager::SetSampleRate(double timestamp,
                                      int sample_rate) noexcept {
  if (sample_rate_ != sample_rate) {
    // Note that the sample accurate timing of the existing instrument events
    // could flactuate during this switch, until the given |timestamp|.
    sample_rate_ = sample_rate;
    for (auto& [instrument_id, controller] : controllers_) {
      auto& update_events = update_events_[instrument_id];
      for (const float pitch : controller.pitches) {
        note_off_callback_(instrument_id, timestamp, pitch);
      }
      controller.pitches.clear();
      update_events.emplace(timestamp, DestroyEvent{});
      update_events.emplace(timestamp, CreateEvent{controller.definition});
      for (int i = 0; i < static_cast<int>(controller.params.size()); ++i) {
        update_events.emplace(
            timestamp, SetParamEvent{i, controller.params[i].GetValue()});
      }
    }
  }
}

void InstrumentManager::Update() noexcept {
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
    InstrumentDefinition definition) noexcept
    : definition(std::move(definition)), gain(1.0f), is_muted(false) {
  params.reserve(this->definition.param_definitions.size());
  for (const auto& param_definition : this->definition.param_definitions) {
    params.emplace_back(param_definition);
  }
}

}  // namespace barely
