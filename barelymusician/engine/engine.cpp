#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each `Process` call.
constexpr int kNumMaxTasks = 1000;

// Default playback tempo in bpm.
constexpr double kDefaultPlaybackTempo = 120.0;

// Converts seconds from minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

/// Instrument id-event pair.
using InstrumentIdEventPair = std::pair<Id, InstrumentEvent>;

/// Instrument id-event pair by position map type.
using InstrumentIdEventPairMap = std::multimap<double, InstrumentIdEventPair>;

}  // namespace

Engine::Engine() noexcept
    : runner_(kNumMaxTasks), playback_tempo_(kDefaultPlaybackTempo) {
  transport_.SetUpdateCallback([this](double begin_position,
                                      double end_position) noexcept {
    InstrumentIdEventPairMap id_event_pairs;
    for (auto& sequence_it : sequences_) {
      auto& sequence = sequence_it.second;
      const auto instrument_id = sequence.GetInstrument();
      sequence.SetEventCallback([&](double position, InstrumentEvent event) {
        id_event_pairs.emplace(position,
                               InstrumentIdEventPair{instrument_id, event});
      });
      sequence.Process(begin_position, end_position);
    }
    for (const auto& [position, id_event_pair] : id_event_pairs) {
      const auto& [instrument_id, event] = id_event_pair;
      if (auto* controller = FindOrNull(controllers_, instrument_id)) {
        controller->ProcessEvent(event, transport_.GetTimestamp(position));
      }
    }
  });
}

Id Engine::AddSequence() noexcept {
  const Id sequence_id = ++id_counter_;
  sequences_.emplace(sequence_id, Sequence{});
  return sequence_id;
}

StatusOr<Id> Engine::AddSequenceNote(Id sequence_id, double position,
                                     BarelyNoteDefinition note) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    const Id note_id = ++id_counter_;
    sequence->AddNote(note_id, position, note);
    return note_id;
  }
  return Status::kNotFound;
}

Id Engine::CreateInstrument(BarelyInstrumentDefinition definition,
                            int sample_rate) noexcept {
  const Id instrument_id = ++id_counter_;
  controllers_.emplace(instrument_id, InstrumentController{definition});
  runner_.Add([this, instrument_id, definition = std::move(definition),
               sample_rate]() noexcept {
    processors_.emplace(std::piecewise_construct,
                        std::forward_as_tuple(instrument_id),
                        std::forward_as_tuple(definition, sample_rate));
  });
  return instrument_id;
}

Status Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (const auto controller_it = controllers_.find(instrument_id);
      controller_it != controllers_.end()) {
    for (auto& [sequence_id, sequence] : sequences_) {
      if (sequence.GetInstrument() == instrument_id) {
        sequence.SetInstrument(kInvalidId);
      }
    }
    controller_it->second.StopAllNotes(transport_.GetTimestamp());
    controllers_.erase(controller_it);
    runner_.Add(
        [this, instrument_id]() noexcept { processors_.erase(instrument_id); });
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<float> Engine::GetInstrumentGain(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->GetGain();
  }
  return Status::kNotFound;
}

StatusOr<Parameter> Engine::GetInstrumentParameter(Id instrument_id,
                                                   int index) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto* parameter = controller->GetParameter(index)) {
      return *parameter;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetSequenceBeginOffset(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetSequenceBeginPosition(
    Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->GetBeginPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetSequenceEndPosition(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->GetEndPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetSequenceLoopBeginOffset(
    Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetSequenceLoopLength(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->GetLoopLength();
  }
  return Status::kNotFound;
}

double Engine::GetPlaybackPosition() const noexcept {
  return transport_.GetPosition();
}

double Engine::GetPlaybackTempo() const noexcept { return playback_tempo_; }

StatusOr<bool> Engine::IsInstrumentMuted(Id instrument_id) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsMuted();
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsInstrumentNoteOn(Id instrument_id,
                                          float pitch) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    return controller->IsNoteOn(pitch);
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsSequenceEmpty(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsSequenceLooping(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->IsLooping();
  }
  return Status::kNotFound;
}

bool Engine::IsPlaying() const noexcept { return transport_.IsPlaying(); }

Status Engine::ProcessInstrument(Id instrument_id, double timestamp,
                                 float* output, int num_channels,
                                 int num_frames) noexcept {
  runner_.Run();
  if (auto* processor = FindOrNull(processors_, instrument_id)) {
    processor->Process(output, num_channels, num_frames, timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllSequenceNotes(Id sequence_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllSequenceNotes(Id sequence_id, double begin_position,
                                      double end_position) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->RemoveAllNotes(begin_position, end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveSequence(Id sequence_id) noexcept {
  if (const auto sequence_it = sequences_.find(sequence_id);
      sequence_it != sequences_.end()) {
    const double timestamp = transport_.GetTimestamp();
    const auto instrument_id = sequence_it->second.GetInstrument();
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      for (const float pitch : sequence_it->second.GetActiveNotes()) {
        controller->ProcessEvent(StopNoteEvent{pitch}, timestamp);
      }
    }
    sequences_.erase(sequence_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveSequenceNote(Id sequence_id, Id note_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    if (sequence->RemoveNote(note_id)) {
      return Status::kOk;
    }
    return Status::kInternal;
  }
  return Status::kNotFound;
}

Status Engine::ResetAllInstrumentParameters(Id instrument_id) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParameters(transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ResetInstrumentParameter(Id instrument_id, int index) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParameter(index, transport_.GetTimestamp())) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentData(Id instrument_id, void* data) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetData(data, transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentGain(Id instrument_id, float gain) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetGain(gain, transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentMuted(Id instrument_id, bool is_muted) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetMuted(is_muted, transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentNoteOffCallback(
    Id instrument_id, BarelyInstrument_NoteOffCallback note_off_callback,
    void* user_data) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOffCallback(note_off_callback, user_data);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentNoteOnCallback(
    Id instrument_id, BarelyInstrument_NoteOnCallback note_on_callback,
    void* user_data) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOnCallback(note_on_callback, user_data);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentParameter(Id instrument_id, int index,
                                      float value) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParameter(index, value, transport_.GetTimestamp())) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceBeginOffset(Id sequence_id,
                                      double begin_offset) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceBeginPosition(Id sequence_id,
                                        double begin_position) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetBeginPosition(begin_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceEndPosition(Id sequence_id,
                                      double end_position) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetEndPosition(end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceLoop(Id sequence_id, bool loop) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetLooping(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceInstrument(Id sequence_id,
                                     Id instrument_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    if (sequence->GetInstrument() != instrument_id) {
      if (auto* controller =
              FindOrNull(controllers_, sequence->GetInstrument())) {
        for (const float pitch : sequence->GetActiveNotes()) {
          controller->ProcessEvent(StopNoteEvent{pitch},
                                   transport_.GetTimestamp());
        }
      }
      sequence->SetInstrument(instrument_id);
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceLoopBeginOffset(Id sequence_id,
                                          double loop_begin_offset) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetSequenceLoopLength(Id sequence_id,
                                     double loop_length) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetPlaybackBeatCallback(BeatCallback beat_callback) noexcept {
  transport_.SetBeatCallback(std::move(beat_callback));
}

void Engine::SetPlaybackPosition(double position) noexcept {
  transport_.SetPosition(position);
}

void Engine::SetPlaybackTempo(double tempo) noexcept {
  playback_tempo_ = std::max(tempo, 0.0);
}

Status Engine::StartInstrumentNote(Id instrument_id, float pitch,
                                   float intensity) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StartNote(pitch, intensity, transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::StartPlayback() noexcept { transport_.Start(); }

Status Engine::StopAllInstrumentNotes(Id instrument_id) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StopAllNotes(transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::StopInstrumentNote(Id instrument_id, float pitch) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->StopNote(pitch, transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::StopPlayback() noexcept {
  for (auto& [sequence_id, sequence] : sequences_) {
    sequence.Stop();
  }
  transport_.Stop();
  for (auto& [instrument_id, controller] : controllers_) {
    controller.StopAllNotes(transport_.GetTimestamp());
  }
}

void Engine::Update(double timestamp) noexcept {
  // TODO: Include conductor tempo adjustment.
  transport_.SetTempo(playback_tempo_ * kMinutesFromSeconds);
  transport_.Update(timestamp);

  std::unordered_map<Id, std::multimap<double, InstrumentEvent>> update_events;
  for (auto& [instrument_id, controller] : controllers_) {
    if (!controller.GetEvents().empty()) {
      update_events.emplace(instrument_id,
                            std::exchange(controller.GetEvents(), {}));
    }
  }
  if (!update_events.empty()) {
    runner_.Add([this, update_events = std::move(update_events)]() noexcept {
      for (const auto& [instrument_id, events] : update_events) {
        if (auto* processor = FindOrNull(processors_, instrument_id)) {
          processor->AddEvents(events);
        }
      }
    });
  }
}

}  // namespace barelyapi
