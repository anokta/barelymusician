#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
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
constexpr int kNumMaxTasks = 1000;

}  // namespace

InstrumentManager::InstrumentManager()
    : task_runner_(kNumMaxTasks),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {}

void InstrumentManager::Create(Id instrument_id, double timestamp,
                               InstrumentDefinition definition,
                               InstrumentParamDefinitions param_definitions) {
  InstrumentController controller;
  for (auto& param_definition : param_definitions) {
    controller.params.emplace(param_definition.id,
                              InstrumentParam(std::move(param_definition)));
  }
  InstrumentProcessor processor;
  processor.events.emplace(timestamp, CreateEvent{std::move(definition)});
  for (const auto& [id, param] : controller.params) {
    processor.events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
  }
  main_events_.emplace(
      timestamp, [this, instrument_id, controller = std::move(controller),
                  processor = std::move(processor)]() {
        if (controllers_.emplace(instrument_id, std::move(controller)).second) {
          audio_events_.emplace_back(
              [this, instrument_id, processor = std::move(processor)]() {
                processors_.emplace(instrument_id, std::move(processor));
              });
        } else {
          LOG(ERROR) << "Instrument id already exists: " << instrument_id;
        }
      });
}

void InstrumentManager::Destroy(Id instrument_id, double timestamp) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp]() {
    if (controllers_.erase(instrument_id) > 0) {
      audio_events_.emplace_back([this, instrument_id, timestamp]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->events.emplace(timestamp, DestroyEvent{});
        }
      });
    } else {
      LOG(ERROR) << "Instrument id does not exist: " << instrument_id;
    }
  });
}

const std::unordered_set<float>* InstrumentManager::GetAllNotes(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return &controller->pitches;
  }
  return nullptr;
}

const std::unordered_map<int, InstrumentParam>* InstrumentManager::GetAllParams(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return &controller->params;
  }
  return nullptr;
}

const InstrumentParam* InstrumentManager::GetParam(Id instrument_id,
                                                   int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return FindOrNull(controller->params, param_id);
  }
  return nullptr;
}

bool InstrumentManager::IsNoteOn(Id instrument_id, float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->pitches.find(note_pitch) != controller->pitches.cend();
  }
  return false;
}

void InstrumentManager::Process(Id instrument_id, double timestamp,
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
  } else {
    LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    std::fill_n(output, num_channels * num_frames, 0.0f);
  }
}

void InstrumentManager::ResetAllParams(double timestamp) {
  main_events_.emplace(timestamp, [this, timestamp]() {
    std::unordered_map<Id, InstrumentEvents> instrument_events;
    for (auto& [instrument_id, controller] : controllers_) {
      auto& events =
          instrument_events.emplace(instrument_id, InstrumentEvents{})
              .first->second;
      for (auto& [id, param] : controller.params) {
        param.ResetValue();
        events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
      }
    }
    audio_events_.emplace_back(
        [this, instrument_events = std::move(instrument_events)]() mutable {
          for (auto& [instrument_id, events] : instrument_events) {
            if (auto* processor = FindOrNull(processors_, instrument_id)) {
              processor->events.merge(std::move(events));
            }
          }
        });
  });
}

void InstrumentManager::ResetAllParams(Id instrument_id, double timestamp) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp]() {
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      InstrumentEvents events;
      for (auto& [id, param] : controller->params) {
        param.ResetValue();
        events.emplace(timestamp, SetParamEvent{id, param.GetValue()});
      }
      SetProcessorEvents(instrument_id, std::move(events));
    } else {
      LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    }
  });
}

void InstrumentManager::ResetParam(Id instrument_id, double timestamp,
                                   int param_id) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp, param_id]() {
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      if (auto* param = FindOrNull(controller->params, param_id)) {
        param->ResetValue();
        SetProcessorEvents(
            instrument_id,
            InstrumentEvents{
                {timestamp, SetParamEvent{param_id, param->GetValue()}}});
      }
    } else {
      LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    }
  });
}

void InstrumentManager::SetAllNotesOff(double timestamp) {
  main_events_.emplace(timestamp, [this, timestamp]() {
    std::unordered_map<Id, InstrumentEvents> instrument_events;
    for (auto& [instrument_id, controller] : controllers_) {
      auto& events =
          instrument_events.emplace(instrument_id, InstrumentEvents{})
              .first->second;
      for (const float pitch : controller.pitches) {
        if (note_off_callback_) {
          note_off_callback_(instrument_id, timestamp, pitch);
        }
        events.emplace(timestamp, SetNoteOffEvent{pitch});
      }
      controller.pitches.clear();
    }
    audio_events_.emplace_back(
        [this, instrument_events = std::move(instrument_events)]() mutable {
          for (auto& [instrument_id, events] : instrument_events) {
            if (auto* processor = FindOrNull(processors_, instrument_id)) {
              processor->events.merge(std::move(events));
            }
          }
        });
  });
}

void InstrumentManager::SetAllNotesOff(Id instrument_id, double timestamp) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp]() {
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      InstrumentEvents events;
      for (const float pitch : controller->pitches) {
        if (note_off_callback_) {
          note_off_callback_(instrument_id, timestamp, pitch);
        }
        events.emplace(timestamp, SetNoteOffEvent{pitch});
      }
      controller->pitches.clear();
      SetProcessorEvents(instrument_id, std::move(events));
    } else {
      LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    }
  });
}

void InstrumentManager::SetCustomData(Id instrument_id, double timestamp,
                                      std::any custom_data) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp,
                                   custom_data = std::move(custom_data)]() {
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      SetProcessorEvents(
          instrument_id,
          InstrumentEvents{
              {timestamp, SetCustomDataEvent{std::move(custom_data)}}});
    } else {
      LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    }
  });
}

void InstrumentManager::SetNoteOff(Id instrument_id, double timestamp,
                                   float note_pitch) {
  main_events_.emplace(
      timestamp, [this, instrument_id, timestamp, note_pitch]() {
        if (auto* controller = FindOrNull(controllers_, instrument_id)) {
          if (controller->pitches.erase(note_pitch) > 0) {
            if (note_off_callback_) {
              note_off_callback_(instrument_id, timestamp, note_pitch);
            }
            SetProcessorEvents(
                instrument_id,
                InstrumentEvents{{timestamp, SetNoteOffEvent{note_pitch}}});
          } else {
            LOG(WARNING) << "Instrument note already off: " << note_pitch;
          }
        } else {
          LOG(ERROR) << "Invalid instrument id: " << instrument_id;
        }
      });
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentManager::SetNoteOn(Id instrument_id, double timestamp,
                                  float note_pitch, float note_intensity) {
  main_events_.emplace(timestamp, [this, instrument_id, timestamp, note_pitch,
                                   note_intensity]() {
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      if (controller->pitches.emplace(note_pitch).second) {
        if (note_on_callback_) {
          note_on_callback_(instrument_id, timestamp, note_pitch,
                            note_intensity);
        }
        SetProcessorEvents(
            instrument_id,
            InstrumentEvents{
                {timestamp, SetNoteOnEvent{note_pitch, note_intensity}}});
      } else {
        LOG(WARNING) << "Instrument note already on: " << note_pitch;
      }
    } else {
      LOG(ERROR) << "Invalid instrument id: " << instrument_id;
    }
  });
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void InstrumentManager::SetParam(Id instrument_id, double timestamp,
                                 int param_id, float param_value) {
  main_events_.emplace(
      timestamp, [this, instrument_id, timestamp, param_id, param_value]() {
        if (auto* controller = FindOrNull(controllers_, instrument_id)) {
          if (auto* param = FindOrNull(controller->params, param_id)) {
            param->SetValue(param_value);
            SetProcessorEvents(
                instrument_id,
                InstrumentEvents{
                    {timestamp, SetParamEvent{param_id, param->GetValue()}}});
          } else {
            LOG(WARNING) << "Instrument parameter does not exist: " << param_id;
          }
        } else {
          LOG(ERROR) << "Invalid instrument id: " << instrument_id;
        }
      });
}

void InstrumentManager::Update(double timestamp) {
  auto begin = main_events_.begin();
  auto end = main_events_.upper_bound(timestamp);
  for (auto it = begin; it != end; ++it) {
    it->second();
  }
  main_events_.erase(begin, end);
  // TODO: ugly, cleanup.
  task_runner_.Add([this, events = std::exchange(audio_events_, {})]() {
    for (const auto& e : events) {
      e();
    }
  });
}

void InstrumentManager::SetProcessorEvents(Id instrument_id,
                                           InstrumentEvents events) {
  audio_events_.emplace_back(
      [this, instrument_id, events = std::move(events)]() mutable {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->events.merge(std::move(events));
        }
      });
}

}  // namespace barelyapi
