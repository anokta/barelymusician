#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/common_utils.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each |Process| call.
constexpr int kNumMaxTasks = 5000;

// Returns number of seconds for the given number of |beats| and |tempo|.
double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

}  // namespace

Engine::Engine()
    : is_playing_(false),
      timestamp_(0.0),
      id_counter_(0),
      task_runner_(kNumMaxTasks),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {}

int Engine::CreateInstrument(InstrumentDefinition definition,
                             InstrumentParamDefinitions param_definitions) {
  const int instrument_id = ++id_counter_;
  InstrumentController controller(param_definitions);
  task_runner_.Add([this, instrument_id, definition = std::move(definition),
                    params = controller.GetAllParams()]() {
    InstrumentProcessor processor(std::move(definition));
    for (const auto& [id, value] : params) {
      processor.SetData(timestamp_, Param{id, value});
    }
    processors_.emplace(instrument_id, std::move(processor));
  });
  controllers_.emplace(instrument_id, std::move(controller));
  return instrument_id;
}

Status Engine::DestroyInstrument(int instrument_id) {
  if (const auto it = controllers_.find(instrument_id);
      it != controllers_.end()) {
    if (note_off_callback_) {
      for (const auto& note_pitch : it->second.GetAllNotes()) {
        note_off_callback_(timestamp_, instrument_id, note_pitch);
      }
    }
    controllers_.erase(it);
    task_runner_.Add(
        [this, instrument_id]() { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::vector<float>> Engine::GetAllInstrumentNotes(
    int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllNotes();
  }
  return Status::kNotFound;
}

StatusOr<std::vector<Param>> Engine::GetAllInstrumentParams(
    int instrument_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetAllParams();
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetInstrumentParam(int instrument_id,
                                           int param_id) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const float* value = controller->GetParam(param_id)) {
      return *value;
    }
  }
  return Status::kNotFound;
}

double Engine::GetPlaybackPosition() const { return clock_.GetPosition(); }

double Engine::GetPlaybackTempo() const { return clock_.GetTempo(); }

StatusOr<bool> Engine::IsInstrumentNoteOn(int instrument_id,
                                          float note_pitch) const {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(note_pitch);
  }
  return Status::kNotFound;
}

bool Engine::IsPlaying() const { return is_playing_; }

Status Engine::ProcessInstrument(int instrument_id, double timestamp,
                                 float* output, int num_channels,
                                 int num_frames) {
  task_runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(sample_rate_, timestamp, output, num_channels,
                       num_frames);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllScheduledInstrumentNotes(int instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->RemoveAllScheduledData();
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::ResetAllInstrumentParams() {
  for (auto& [instrument_id, controller] : controllers_) {
    controller.ResetAllParams();
    task_runner_.Add([this, instrument_id = instrument_id,
                      timestamp = timestamp_,
                      params = controller.GetAllParams()]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& [id, value] : params) {
          processor->SetData(timestamp, Param{id, value});
        }
      }
    });
  }
}

Status Engine::ResetAllInstrumentParams(int instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParams();
    task_runner_.Add([this, instrument_id, timestamp = timestamp_,
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

Status Engine::ResetInstrumentParam(int instrument_id, int param_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(param_id)) {
      SetProcessorData(instrument_id,
                       Param{param_id, *controller->GetParam(param_id)});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::ScheduleInstrumentNote(int instrument_id, double note_position,
                                      double note_duration, float note_pitch,
                                      float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ScheduleNote(note_position, note_duration, note_pitch,
                             note_intensity);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetAllInstrumentNotesOff() {
  for (auto& [instrument_id, controller] : controllers_) {
    auto notes = controller.GetAllNotes();
    controller.SetAllNotesOff();
    if (note_off_callback_) {
      for (const auto& note_pitch : notes) {
        note_off_callback_(timestamp_, instrument_id, note_pitch);
      }
    }
    task_runner_.Add([this, instrument_id = instrument_id,
                      timestamp = timestamp_, notes = std::move(notes)]() {
      if (auto* processor = FindOrNull(processors_, instrument_id)) {
        for (const auto& note_pitch : notes) {
          processor->SetData(timestamp, NoteOff{note_pitch});
        }
      }
    });
  }
}

Status Engine::SetAllInstrumentNotesOff(int instrument_id) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    auto notes = controller->GetAllNotes();
    controller->SetAllNotesOff();
    if (note_off_callback_) {
      for (const auto& note_pitch : notes) {
        note_off_callback_(timestamp_, instrument_id, note_pitch);
      }
    }
    task_runner_.Add([this, instrument_id, timestamp = timestamp_,
                      notes = std::move(notes)]() {
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

void Engine::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

Status Engine::SetCustomInstrumentData(int instrument_id,

                                       void* custom_data) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    SetProcessorData(instrument_id, CustomData{custom_data});
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentNoteOff(int instrument_id, float note_pitch) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOff(note_pitch)) {
      if (note_off_callback_) {
        note_off_callback_(timestamp_, instrument_id, note_pitch);
      }
      SetProcessorData(instrument_id, NoteOff{note_pitch});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentNoteOn(int instrument_id, float note_pitch,
                                   float note_intensity) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetNoteOn(note_pitch)) {
      if (note_on_callback_) {
        note_on_callback_(timestamp_, instrument_id, note_pitch,
                          note_intensity);
      }
      SetProcessorData(instrument_id, NoteOn{note_pitch, note_intensity});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentParam(int instrument_id, int param_id,
                                  float param_value) {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(param_id, param_value)) {
      SetProcessorData(instrument_id,
                       Param{param_id, *controller->GetParam(param_id)});
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
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
  task_runner_.Add([this, sample_rate]() { sample_rate_ = sample_rate; });
}

void Engine::StartPlayback() { is_playing_ = true; }

void Engine::StopPlayback() {
  SetAllInstrumentNotesOff();
  is_playing_ = false;
}

void Engine::Update(double timestamp) {
  if (is_playing_) {
    const double begin_position = clock_.GetPosition();
    clock_.UpdatePosition(timestamp - timestamp_);
    const double end_position = clock_.GetPosition();
    if (begin_position < end_position) {
      const double tempo = clock_.GetTempo();
      // Trigger beats.
      if (beat_callback_) {
        for (double beat = std::ceil(begin_position); beat < end_position;
             ++beat) {
          const double beat_timestamp =
              timestamp_ + SecondsFromBeats(tempo, beat - end_position);
          beat_callback_(beat_timestamp, static_cast<int>(beat));
        }
      }
      // Trigger messages.
      for (auto& it : controllers_) {
        const int instrument_id = it.first;
        auto& controller = it.second;
        auto scheduled_data =
            controller.GetAllScheduledData(begin_position, end_position);
        for (auto& data_it : scheduled_data) {
          data_it.first =
              timestamp_ +
              SecondsFromBeats(tempo, data_it.first - begin_position);
          std::visit(InstrumentDataVisitor{
                         [&](const NoteOff& note_off) {
                           if (controller.SetNoteOff(note_off.pitch) &&
                               note_off_callback_) {
                             note_off_callback_(data_it.first, instrument_id,
                                                note_off.pitch);
                           }
                         },
                         [&](const NoteOn& note_on) {
                           if (controller.SetNoteOn(note_on.pitch) &&
                               note_on_callback_) {
                             note_on_callback_(data_it.first, instrument_id,
                                               note_on.pitch,
                                               note_on.intensity);
                           }
                         },
                         [](const auto&) {}},
                     data_it.second);
        }
        task_runner_.Add([this, instrument_id = instrument_id,
                          scheduled_data = std::move(scheduled_data)]() {
          if (auto* processor = FindOrNull(processors_, instrument_id)) {
            for (const auto& [timestamp, data] : scheduled_data) {
              processor->SetData(timestamp, data);
            }
          }
        });
      }
    }
  }
  timestamp_ = timestamp;
}

void Engine::SetProcessorData(int instrument_id, InstrumentData data) {
  task_runner_.Add(
      [this, timestamp = timestamp_, instrument_id, data = std::move(data)]() {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->SetData(timestamp, std::move(data));
        }
      });
}

}  // namespace barelyapi
