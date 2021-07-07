#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <unordered_map>
#include <utility>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"
#include "barelymusician/engine/instrument_param_definition.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 8000;

}  // namespace

InstrumentManager::InstrumentManager()
    : task_runner_(kNumMaxTasks),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {}

bool InstrumentManager::Create(Id instrument_id, double timestamp,
                               InstrumentDefinition definition,
                               InstrumentParamDefinitions param_definitions) {
  InstrumentController controller(timestamp, std::move(definition),
                                  std::move(param_definitions));
  return controllers_.emplace(instrument_id, std::move(controller)).second;
}

bool InstrumentManager::Destroy(Id instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    if (note_off_callback_) {
      for (const float pitch : controller->data->pitches) {
        note_off_callback_(instrument_id, timestamp, pitch);
      }
    }
    controller->events.emplace(timestamp, DestroyEvent{});
    return true;
  }
  return false;
}

const std::unordered_set<float>* InstrumentManager::GetAllNotes(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return nullptr;
    }
    return &controller->data->pitches;
  }
  return nullptr;
}

const std::unordered_map<int, InstrumentParam>* InstrumentManager::GetAllParams(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return nullptr;
    }
    return &controller->data->params;
  }
  return nullptr;
}

const InstrumentParam* InstrumentManager::GetParam(Id instrument_id,
                                                   int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return nullptr;
    }
    return FindOrNull(controller->data->params, param_id);
  }
  return nullptr;
}

bool InstrumentManager::IsNoteOn(Id instrument_id, float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    return controller->data->pitches.find(note_pitch) !=
           controller->data->pitches.cend();
  }
  return false;
}

bool InstrumentManager::Process(Id instrument_id, double timestamp,
                                int sample_rate, float* output,
                                int num_channels, int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    int frame = 0;
    // Process *all* events before the end timestamp.
    auto begin = processor->events.begin();
    auto end = processor->events.lower_bound(
        timestamp + SecondsFromSamples(sample_rate, num_frames));
    // TODO: this seems wasteful? maybe at least encapsulate in Instrument?
    Instrument* instrument = processor->instrument.has_value()
                                 ? &processor->instrument.value()
                                 : nullptr;
    // TODO: what happens if sample rate changes?
    for (auto it = begin; it != end; ++it) {
      const int message_frame =
          SamplesFromSeconds(sample_rate, it->first - timestamp);
      if (frame < message_frame) {
        // TODO: fill zeros on failure here and below.
        if (instrument) {
          instrument->Process(sample_rate, &output[num_channels * frame],
                              num_channels, message_frame - frame);
        }
        frame = message_frame;
      }
      std::visit(
          InstrumentEventVisitor{
              [&](CreateEvent& create) {
                processor->instrument =
                    Instrument(sample_rate, std::move(create.definition));
                instrument = &processor->instrument.value();
              },
              [&](DestroyEvent& /*destroy*/) {
                // TODO: remove processor from the list directly.
                processor->instrument.reset();
                instrument = nullptr;
              },
              [&](SetCustomDataEvent& set_custom_data) {
                if (instrument) {
                  instrument->SetCustomData(std::move(set_custom_data.data));
                }
              },
              [&](SetNoteOffEvent& set_note_off) {
                if (instrument) {
                  instrument->SetNoteOff(set_note_off.pitch);
                }
              },
              [&](SetNoteOnEvent& set_note_on) {
                if (instrument) {
                  instrument->SetNoteOn(set_note_on.pitch,
                                        set_note_on.intensity);
                }
              },
              [&](SetParamEvent& set_param) {
                if (instrument) {
                  instrument->SetParam(set_param.id, set_param.value);
                }
              }},
          it->second);
    }
    processor->events.erase(begin, end);
    // Process the rest of the buffer.
    if (frame < num_frames && instrument) {
      instrument->Process(sample_rate, &output[num_channels * frame],
                          num_channels, num_frames - frame);
    }
    // TODO: erase properly without extra lookup.
    if (!processor->instrument.has_value()) {
      processors_.erase(instrument_id);
    }
    return true;
  }
  return false;
}

void InstrumentManager::SetAllNotesOff(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    if (!controller.data.has_value()) {
      continue;
    }
    for (const float pitch : controller.data->pitches) {
      if (note_off_callback_) {
        note_off_callback_(instrument_id, timestamp, pitch);
      }
      controller.events.emplace(timestamp, SetNoteOffEvent{pitch});
    }
    controller.data->pitches.clear();
  }
}

bool InstrumentManager::SetAllNotesOff(Id instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    for (const float pitch : controller->data->pitches) {
      if (note_off_callback_) {
        note_off_callback_(instrument_id, timestamp, pitch);
      }
      controller->events.emplace(timestamp, SetNoteOffEvent{pitch});
    }
    controller->data->pitches.clear();
    return true;
  }
  return false;
}

void InstrumentManager::SetAllParamsToDefault(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    if (!controller.data.has_value()) {
      continue;
    }
    for (auto& [id, param] : controller.data->params) {
      param.ResetValue();
      controller.events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
    }
  }
}

bool InstrumentManager::SetAllParamsToDefault(Id instrument_id,
                                              double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    for (auto& [id, param] : controller->data->params) {
      param.ResetValue();
      controller->events.emplace(timestamp,
                                 SetParamEvent{id, param.GetValue()});
    }
    return true;
  }
  return false;
}

bool InstrumentManager::SetCustomData(Id instrument_id, double timestamp,
                                      std::any custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    controller->events.emplace(timestamp,
                               SetCustomDataEvent{std::move(custom_data)});
    return true;
  }
  return false;
}

bool InstrumentManager::SetNoteOff(Id instrument_id, double timestamp,
                                   float note_pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    if (controller->data->pitches.erase(note_pitch) > 0) {
      if (note_off_callback_) {
        note_off_callback_(instrument_id, timestamp, note_pitch);
      }
      controller->events.emplace(timestamp, SetNoteOffEvent{note_pitch});
      return true;
    }
  }
  return false;
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

bool InstrumentManager::SetNoteOn(Id instrument_id, double timestamp,
                                  float note_pitch, float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    if (controller->data->pitches.emplace(note_pitch).second) {
      if (note_on_callback_) {
        note_on_callback_(instrument_id, timestamp, note_pitch, note_intensity);
      }
      controller->events.emplace(timestamp,
                                 SetNoteOnEvent{note_pitch, note_intensity});
      return true;
    }
  }
  return false;
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

bool InstrumentManager::SetParam(Id instrument_id, double timestamp,
                                 int param_id, float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    if (auto* param = FindOrNull(controller->data->params, param_id)) {
      param->SetValue(param_value);
      controller->events.emplace(timestamp,
                                 SetParamEvent{param_id, param->GetValue()});
      return true;
    }
  }
  return false;
}

bool InstrumentManager::SetParamToDefault(Id instrument_id, double timestamp,
                                          int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (!controller->data.has_value()) {
      return false;
    }
    if (auto* param = FindOrNull(controller->data->params, param_id)) {
      param->ResetValue();
      controller->events.emplace(timestamp,
                                 SetParamEvent{param_id, param->GetValue()});
      return true;
    }
  }
  return false;
}

void InstrumentManager::Update() {
  std::unordered_map<Id, InstrumentEvents> update_events;
  for (auto& [instrument_id, controller] : controllers_) {
    std::swap(controller.events, update_events[instrument_id]);
  }
  task_runner_.Add([this, update_events = std::move(update_events)]() mutable {
    for (auto& [instrument_id, events] : update_events) {
      processors_[instrument_id].events.merge(std::move(events));
    }
  });
  std::erase_if(controllers_, [](const auto& controller) {
    return !controller.second.data.has_value();
  });
}

}  // namespace barelyapi
