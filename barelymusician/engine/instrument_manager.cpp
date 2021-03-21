#include "barelymusician/engine/instrument_manager.h"

#include <utility>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/logging.h"

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

int InstrumentManager::Create(InstrumentDefinition definition,
                              InstrumentParamDefinitions param_definitions) {
  const int instrument_id = id_generator_->Generate();
  InstrumentController controller(param_definitions);
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = controller.GetAllParams()]() {
    InstrumentProcessor processor(sample_rate_, std::move(definition));
    for (auto& param : params) {
      processor.ScheduleEvent(std::move(param), 0.0);
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
  events_.emplace(instrument_id, std::multimap<double, InstrumentEvent>{});
  return instrument_id;
}

bool InstrumentManager::Destroy(int instrument_id) {
  if (const auto it = controllers_.find(instrument_id);
      it != controllers_.end()) {
    if (note_off_callback_) {
      for (const auto& note_pitch : it->second.GetAllNotes()) {
        note_off_callback_(instrument_id, note_pitch);
      }
    }
    controllers_.erase(it);
    events_.erase(instrument_id);
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return true;
  }
  return false;
}

std::vector<float> InstrumentManager::GetAllNotes(int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return {};
}

std::vector<Param> InstrumentManager::GetAllParams(int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return {};
}

const float* InstrumentManager::GetParam(int instrument_id,
                                         int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetParam(param_id);
  }
  return nullptr;
}

bool InstrumentManager::IsNoteOn(int instrument_id, float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return false;
}

bool InstrumentManager::Process(int instrument_id, float* output,
                                int num_channels, int num_frames,
                                double timestamp) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(output, num_channels, num_frames, timestamp);
    return true;
  }
  return false;
}

void InstrumentManager::ResetAllParams(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    auto* events = FindOrNull(events_, instrument_id);
    for (const auto& [id, value] : controller.GetAllParams()) {
      events->emplace(timestamp, Param{id, *controller.GetDefaultParam(id)});
    }
    // task_runner_.Add([this, instrument_id = instrument_id, timestamp,
    //                   params = controller.GetAllParams()]() {
    //   if (auto* processor = FindOrNull(processors_, instrument_id)) {
    //     for (auto& param : params) {
    //       processor->ScheduleEvent(std::move(param), timestamp);
    //     }
    //   }
    // });
  }
}

bool InstrumentManager::ResetAllParams(int instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    for (const auto& [id, value] : controller->GetAllParams()) {
      events->emplace(timestamp, Param{id, *controller->GetDefaultParam(id)});
    }
    // task_runner_.Add([this, instrument_id, timestamp,
    //                   params = controller->GetAllParams()]() {
    //   if (auto* processor = FindOrNull(processors_, instrument_id)) {
    //     for (auto& param : params) {
    //       processor->ScheduleEvent(std::move(param), timestamp);
    //     }
    //   }
    // });
    return true;
  }
  return false;
}

bool InstrumentManager::ResetParam(int instrument_id, int param_id,
                                   double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    // TODO: Do reset param on Update!
    if (controller->ResetParam(param_id)) {
      auto* events = FindOrNull(events_, instrument_id);
      events->emplace(timestamp,
                      Param{param_id, *controller->GetParam(param_id)});
      // ScheduleProcessorEvent(instrument_id,
      //                        Param{param_id,
      //                        *controller->GetParam(param_id)}, timestamp);
      return true;
    }
  }
  return false;
}

void InstrumentManager::SetAllNotesOff(double timestamp) {
  for (auto& [instrument_id, controller] : controllers_) {
    auto* events = FindOrNull(events_, instrument_id);
    for (const auto& note : controller.GetAllNotes()) {
      events->emplace(timestamp, NoteOff{note});
    }
    // auto notes = controller.GetAllNotes();
    // controller.SetAllNotesOff();
    // if (note_off_callback_) {
    //   for (const auto& note_pitch : notes) {
    //     note_off_callback_(instrument_id, note_pitch);
    //   }
    // }
    // task_runner_.Add([this, instrument_id = instrument_id, timestamp,
    //                   notes = std::move(notes)]() {
    //   if (auto* processor = FindOrNull(processors_, instrument_id)) {
    //     for (const auto& note_pitch : notes) {
    //       processor->ScheduleEvent(NoteOff{note_pitch}, timestamp);
    //     }
    //   }
    // });
  }
}

bool InstrumentManager::SetAllNotesOff(int instrument_id, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    // TODO: should do GetAllNotes at scheduled time as well! Most likely should
    // have InstrumentEvent vs InstrumentProcessorEvent.
    for (const auto& note : controller->GetAllNotes()) {
      events->emplace(timestamp, NoteOff{note});
    }
    // auto notes = controller->GetAllNotes();
    // controller->SetAllNotesOff();
    // if (note_off_callback_) {
    //   for (const auto& note_pitch : notes) {
    //     note_off_callback_(instrument_id, note_pitch);
    //   }
    // }
    // task_runner_.Add(
    //     [this, instrument_id, timestamp, notes = std::move(notes)]() {
    //       if (auto* processor = FindOrNull(processors_, instrument_id)) {
    //         for (const auto& note_pitch : notes) {
    //           processor->ScheduleEvent(NoteOff{note_pitch}, timestamp);
    //         }
    //       }
    //     });
    return true;
  }
  return false;
}

bool InstrumentManager::SetCustomData(int instrument_id, void* custom_data,
                                      double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    // ScheduleProcessorEvent(instrument_id, CustomData{custom_data},
    // timestamp);
    auto* events = FindOrNull(events_, instrument_id);
    events->emplace(timestamp, CustomData{custom_data});
    return true;
  }
  return false;
}

bool InstrumentManager::SetEvents(
    int instrument_id, std::multimap<double, InstrumentEvent> input_events) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    events->merge(input_events);
    // for (auto it = events.begin(); it != events.end();) {
    //   bool success = false;
    //   std::visit(InstrumentEventVisitor{
    //                  [&](const CustomData& /*custom_data*/) { success = true;
    //                  },
    //                  [&](const NoteOff& note_off) {
    //                    if (controller->SetNoteOff(note_off.pitch)) {
    //                      if (note_off_callback_) {
    //                        note_off_callback_(instrument_id, note_off.pitch);
    //                      }
    //                      success = true;
    //                    }
    //                  },
    //                  [&](const NoteOn& note_on) {
    //                    if (controller->SetNoteOn(note_on.pitch)) {
    //                      if (note_on_callback_) {
    //                        note_on_callback_(instrument_id, note_on.pitch,
    //                                          note_on.intensity);
    //                      }
    //                      success = true;
    //                    }
    //                  },
    //                  [&](const Param& param) {
    //                    success = controller->SetParam(param.id, param.value);
    //                  }},
    //              it->second);
    //   if (success) {
    //     ++it;
    //   } else {
    //     it = events.erase(it);
    //   }
    // }
    // task_runner_.Add([this, instrument_id, events = std::move(events)]() {
    //   if (auto* processor = FindOrNull(processors_, instrument_id)) {
    //     processor->ScheduleEvents(events);
    //   }
    // });
    return true;
  }
  return false;
}

bool InstrumentManager::SetNoteOff(int instrument_id, float note_pitch,
                                   double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    events->emplace(timestamp, NoteOff{note_pitch});
    // if (controller->SetNoteOff(note_pitch)) {
    //   if (note_off_callback_) {
    //     note_off_callback_(instrument_id, note_pitch);
    //   }
    //   ScheduleProcessorEvent(instrument_id, NoteOff{note_pitch}, timestamp);
    return true;
  }
  // }
  return false;
}

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

bool InstrumentManager::SetNoteOn(int instrument_id, float note_pitch,
                                  float note_intensity, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    events->emplace(timestamp, NoteOn{note_pitch, note_intensity});
    // if (controller->SetNoteOn(note_pitch)) {
    //   if (note_on_callback_) {
    //     note_on_callback_(instrument_id, note_pitch, note_intensity);
    //   }
    //   ScheduleProcessorEvent(instrument_id, NoteOn{note_pitch,
    //   note_intensity},
    //                          timestamp);
    return true;
    // }
  }
  return false;
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

bool InstrumentManager::SetParam(int instrument_id, int param_id,
                                 float param_value, double timestamp) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto* events = FindOrNull(events_, instrument_id);
    events->emplace(timestamp, Param{param_id, param_value});
    // if (controller->SetParam(param_id, param_value)) {
    //   ScheduleProcessorEvent(instrument_id,
    //                          Param{param_id,
    //                          *controller->GetParam(param_id)}, timestamp);
    return true;
    // }
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

// void InstrumentManager::ScheduleProcessorEvent(int instrument_id,
//                                                InstrumentEvent event,
//                                                double timestamp) {
//   task_runner_.Add(
//       [this, instrument_id, event = std::move(event), timestamp]() {
//         if (auto* processor = FindOrNull(processors_, instrument_id)) {
//           processor->ScheduleEvent(std::move(event), timestamp);
//         }
//       });
// }

void InstrumentManager::Update(double timestamp) {
  for (auto& controller_it : controllers_) {
    auto& instrument_id = controller_it.first;
    auto& controller = controller_it.second;
    auto* events = FindOrNull(events_, instrument_id);

    std::multimap<double, InstrumentEvent> shuttle;

    auto begin = events->begin();
    auto end = events->upper_bound(timestamp);
    for (auto it = begin; it != end; ++it) {
      bool success = false;
      std::visit(InstrumentEventVisitor{
                     [&](const CustomData& /*custom_data*/) { success = true; },
                     [&](const NoteOff& note_off) {
                       if (controller.SetNoteOff(note_off.pitch)) {
                         if (note_off_callback_) {
                           note_off_callback_(instrument_id, note_off.pitch);
                         }
                         success = true;
                       }
                     },
                     [&](const NoteOn& note_on) {
                       if (controller.SetNoteOn(note_on.pitch)) {
                         if (note_on_callback_) {
                           note_on_callback_(instrument_id, note_on.pitch,
                                             note_on.intensity);
                         }
                         success = true;
                       }
                     },
                     [&](Param& param) {
                       if (controller.SetParam(param.id, param.value)) {
                         param.value = *controller.GetParam(param.id);
                         success = true;
                       }
                     }},
                 it->second);
      if (success) {
        shuttle.insert(std::move(*it));
      }
    }
    events->erase(begin, end);
    task_runner_.Add([this, instrument_id, shuttle = std::move(shuttle)]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        processor->ScheduleEvents(shuttle);
      }
    });
  }
}

}  // namespace barelyapi
