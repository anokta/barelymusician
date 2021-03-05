#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/common_utils.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 8000;

}  // namespace

Engine::Engine(int sample_rate)
    : id_counter_(0),
      task_runner_(kNumMaxTasks),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  task_runner_.Add([this, sample_rate]() { sample_rate_ = sample_rate; });
}

int Engine::CreateInstrument(InstrumentDefinition definition,
                             InstrumentParamDefinitions param_definitions) {
  const int instrument_id = ++id_counter_;
  InstrumentController controller(param_definitions);
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = controller.GetAllParams()]() {
    InstrumentProcessor processor(sample_rate_, std::move(definition));
    for (auto& param : params) {
      processor.SetData(clock_.GetTimestamp(), std::move(param));
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
  return instrument_id;
}

bool Engine::DestroyInstrument(int instrument_id) {
  if (const auto it = controllers_.find(instrument_id);
      it != controllers_.end()) {
    if (note_off_callback_) {
      for (const auto& note_pitch : it->second.GetAllNotes()) {
        note_off_callback_(instrument_id, clock_.GetTimestamp(), note_pitch);
      }
    }
    controllers_.erase(it);
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return true;
  }
  return false;
}

std::vector<float> Engine::GetAllInstrumentNotes(int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return {};
}

std::vector<Param> Engine::GetAllInstrumentParams(int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return {};
}

const float* Engine::GetInstrumentParam(int instrument_id, int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetParam(param_id);
  }
  return nullptr;
}

double Engine::GetPlaybackPosition() const { return clock_.GetPosition(); }

double Engine::GetPlaybackTempo() const { return clock_.GetTempo(); }

bool Engine::IsInstrumentNoteOn(int instrument_id, float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return false;
}

bool Engine::IsPlaying() const { return clock_.IsPlaying(); }

bool Engine::ProcessInstrument(int instrument_id, double timestamp,
                               float* output, int num_channels,
                               int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(timestamp, output, num_channels, num_frames);
    return true;
  }
  return false;
}

void Engine::RemoveAllScheduledInstrumentNotes() {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.RemoveAllScheduledData();
  }
}

bool Engine::RemoveAllScheduledInstrumentNotes(int instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->RemoveAllScheduledData();
    return true;
  }
  return false;
}

void Engine::ResetAllInstrumentParams() {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.ResetAllParams();
    task_runner_.Add([this, instrument_id = instrument_id,
                      timestamp = clock_.GetTimestamp(),
                      params = controller.GetAllParams()]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (auto& param : params) {
          processor->SetData(timestamp, std::move(param));
        }
      }
    });
  }
}

bool Engine::ResetAllInstrumentParams(int instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParams();
    task_runner_.Add([this, instrument_id, timestamp = clock_.GetTimestamp(),
                      params = controller->GetAllParams()]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (auto& param : params) {
          processor->SetData(timestamp, std::move(param));
        }
      }
    });
    return true;
  }
  return false;
}

bool Engine::ResetInstrumentParam(int instrument_id, int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(param_id)) {
      SetProcessorData(instrument_id,
                       Param{param_id, *controller->GetParam(param_id)});
      return true;
    }
  }
  return false;
}

bool Engine::ScheduleInstrumentNote(int instrument_id,
                                    double note_begin_position,
                                    double note_end_position, float note_pitch,
                                    float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ScheduleNote(note_begin_position, note_end_position, note_pitch,
                             note_intensity);
    return true;
  }
  return false;
}

void Engine::SetAllInstrumentNotesOff() {
  const double timestamp = clock_.GetTimestamp();
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

bool Engine::SetAllInstrumentNotesOff(int instrument_id) {
  const double timestamp = clock_.GetTimestamp();
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
    return true;
  }
  return false;
}

void Engine::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

bool Engine::SetCustomInstrumentData(int instrument_id, void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    SetProcessorData(instrument_id, CustomData{custom_data});
    return true;
  }
  return false;
}

bool Engine::SetInstrumentNoteOff(int instrument_id, float note_pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOff(note_pitch)) {
      if (note_off_callback_) {
        note_off_callback_(instrument_id, clock_.GetTimestamp(), note_pitch);
      }
      SetProcessorData(instrument_id, NoteOff{note_pitch});
      return true;
    }
  }
  return false;
}

bool Engine::SetInstrumentNoteOn(int instrument_id, float note_pitch,
                                 float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOn(note_pitch)) {
      if (note_on_callback_) {
        note_on_callback_(instrument_id, clock_.GetTimestamp(), note_pitch,
                          note_intensity);
      }
      SetProcessorData(instrument_id, NoteOn{note_pitch, note_intensity});
      return true;
    }
  }
  return false;
}

bool Engine::SetInstrumentParam(int instrument_id, int param_id,
                                float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(param_id, param_value)) {
      SetProcessorData(instrument_id,
                       Param{param_id, *controller->GetParam(param_id)});
      return true;
    }
  }
  return false;
}

void Engine::SetNoteOffCallback(NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}
void Engine::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void Engine::SetPlaybackPosition(double position) {
  SetAllInstrumentNotesOff();
  clock_.SetPosition(position);
}

void Engine::SetPlaybackTempo(double tempo) { clock_.SetTempo(tempo); }

void Engine::SetSampleRate(int sample_rate) {
  SetAllInstrumentNotesOff();
  task_runner_.Add([this, sample_rate]() {
    sample_rate_ = sample_rate;
    for (auto& [instrument_id, processor] : processors_) {
      processor.Reset(sample_rate);
    }
  });
}

void Engine::StartPlayback() { clock_.Start(); }

void Engine::StopPlayback() {
  SetAllInstrumentNotesOff();
  clock_.Stop();
}

void Engine::Update(double timestamp) {
  const double begin_position = clock_.GetPosition();
  clock_.Update(timestamp);
  const double end_position = clock_.GetPosition();
  if (begin_position < end_position) {
    // Trigger beats.
    if (beat_callback_) {
      for (double beat = std::ceil(begin_position); beat < end_position;
           ++beat) {
        beat_callback_(clock_.GetTimestampAtPosition(beat),
                       static_cast<int>(beat));
      }
    }
    // Trigger messages.
    for (auto& it : controllers_) {
      const int instrument_id = it.first;
      auto& controller = it.second;
      auto scheduled_data =
          controller.GetAllScheduledData(begin_position, end_position);
      for (auto& data_it : scheduled_data) {
        data_it.first = clock_.GetTimestampAtPosition(data_it.first);
        std::visit(InstrumentDataVisitor{
                       [&](const NoteOff& note_off) {
                         if (controller.SetNoteOff(note_off.pitch) &&
                             note_off_callback_) {
                           note_off_callback_(instrument_id, data_it.first,
                                              note_off.pitch);
                         }
                       },
                       [&](const NoteOn& note_on) {
                         if (controller.SetNoteOn(note_on.pitch) &&
                             note_on_callback_) {
                           note_on_callback_(instrument_id, data_it.first,
                                             note_on.pitch, note_on.intensity);
                         }
                       },
                       [](const auto&) {}},
                   data_it.second);
      }
      task_runner_.Add([this, instrument_id = instrument_id,
                        scheduled_data = std::move(scheduled_data)]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          for (const auto& [data_timestamp, data] : scheduled_data) {
            processor->SetData(data_timestamp, data);
          }
        }
      });
    }
  }
}

void Engine::SetProcessorData(int instrument_id, InstrumentData data) {
  task_runner_.Add([this, timestamp = clock_.GetTimestamp(), instrument_id,
                    data = std::move(data)]() {
    if (auto* processor = FindOrNull(processors_, instrument_id)) {
      processor->SetData(timestamp, std::move(data));
    }
  });
}

}  // namespace barelyapi
