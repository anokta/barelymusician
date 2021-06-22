#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <utility>
#include <vector>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/common/id.h"
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

// InstrumentProcessorEvents BuildAllNotesOffEvents(double timestamp,
//                                                  std::vector<float> pitches)
//                                                  {
//   InstrumentProcessorEvents events;
//   for (const auto& pitch : pitches) {
//     events.emplace(timestamp, SetNoteOff{pitch});
//   }
//   return events;
// }

InstrumentProcessorEvents BuildParamEvents(
    double timestamp, const std::unordered_map<int, InstrumentParam>& params) {
  InstrumentProcessorEvents events;
  for (const auto& [id, param] : params) {
    events.emplace(timestamp, SetParam{id, param.GetValue()});
  }
  return events;
}

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
      definition, param_definitions,
      [this, instrument_id](float pitch) {
        if (note_off_callback_) {
          note_off_callback_(instrument_id, pitch);
        }
      },
      [this, instrument_id](float pitch, float intensity) {
        if (note_on_callback_) {
          note_on_callback_(instrument_id, pitch, intensity);
        }
      });
  task_runner_.Add(
      [this, instrument_id, sample_rate = sample_rate_,
       definition = std::move(definition),
       param_events = BuildParamEvents(0.0, controller.GetAllParams())]() {
        InstrumentProcessor processor(sample_rate, std::move(definition));
        processor.Schedule(std::move(param_events));
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
    const auto& pitches = controller->GetAllNotes();
    return std::vector<float>{pitches.begin(), pitches.end()};
  }
  return {};
}

std::vector<InstrumentParam> InstrumentManager::GetAllParams(
    Id instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    const auto& params = controller->GetAllParams();
    std::vector<InstrumentParam> ret;
    ret.reserve(params.size());
    std::transform(params.begin(), params.end(), std::back_inserter(ret),
                   [](const auto& param) { return param.second; });
    return ret;
  }
  return {};
}

const InstrumentParam* InstrumentManager::GetParam(Id instrument_id,
                                                   int param_id) const {
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

bool InstrumentManager::Process(Id instrument_id, double timestamp,
                                float* output, int num_channels,
                                int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return true;
  }
  return false;
}

bool InstrumentManager::SetEvent(Id instrument_id, double timestamp,
                                 InstrumentControllerEvent event) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->Schedule(timestamp, std::move(event));
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

void InstrumentManager::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentManager::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void InstrumentManager::SetSampleRate(int sample_rate) {
  sample_rate_ = sample_rate;
  for (auto& [instrument_id, controller] : controllers_) {
    // TODO: nope - needs to happen before Reset call below.
    controller.Schedule(0.0, SetAllNotesOff{});

    task_runner_.Add([this, sample_rate, instrument_id = instrument_id,
                      definition = controller.GetDefinition(),
                      param_events =
                          BuildParamEvents(0.0, controller.GetAllParams())]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        *processor = InstrumentProcessor(sample_rate, std::move(definition));
        processor->Schedule(std::move(param_events));
      }
    });
  }
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
