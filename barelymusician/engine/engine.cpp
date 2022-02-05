#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/sequence.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"
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

// Dummy beat callback function that does nothing.
void NoopBeatCallback(double /*position*/, double /*timestamp*/) noexcept {}

// Dummy note off callback that does nothing.
void NoopNoteOffCallback(float /*pitch*/, double /*timestamp*/) noexcept {}

// Dummy note on callback that does nothing.
void NoopNoteOnCallback(float /*pitch*/, float /*intensity*/,
                        double /*timestamp*/) noexcept {}

}  // namespace

Engine::Engine() noexcept
    : runner_(kNumMaxTasks),
      playback_tempo_(kDefaultPlaybackTempo),
      transport_(&NoopBeatCallback, [this](double begin_position,
                                           double end_position) noexcept {
        InstrumentIdEventPairMap id_event_pairs;
        for (auto& performer_it : performers_) {
          auto& performer = performer_it.second;
          const auto instrument_id = performer.instrument_id;
          // Perform active note events.
          for (auto it = performer.active_notes.begin();
               it != performer.active_notes.end();) {
            const auto& [note_begin_position, active_note] = *it;
            double note_end_position = active_note.end_position;
            if (note_end_position < end_position) {
              note_end_position = std::max(begin_position, note_end_position);
            } else if (begin_position < note_begin_position) {
              note_end_position = begin_position;
            } else {
              ++it;
              continue;
            }
            // Perform note off event.
            id_event_pairs.emplace(
                note_end_position,
                InstrumentIdEventPair{instrument_id,
                                      StopNoteEvent{active_note.pitch}});
            it = performer.active_notes.erase(it);
          }

          // Perform sequence events.
          performer.sequence.Process(
              begin_position, end_position,
              [&](double position, const Note& note) noexcept {
                // Get pitch.
                const auto pitch_or = conductor_.TransformNotePitch(note.pitch);
                if (!IsOk(pitch_or)) {
                  return;
                }
                const float pitch = GetStatusOrValue(pitch_or);
                // Get intensity.
                const auto intensity_or =
                    conductor_.TransformNoteIntensity(note.intensity);
                if (!IsOk(intensity_or)) {
                  return;
                }
                const float intensity = GetStatusOrValue(intensity_or);
                // Get duration.
                const auto duration_or =
                    conductor_.TransformNoteDuration(note.duration);
                if (!IsOk(duration_or)) {
                  return;
                }
                const double note_end_position = std::min(
                    position + std::max(GetStatusOrValue(duration_or), 0.0),
                    performer.sequence.GetEndPosition());

                // Perform note on event.
                id_event_pairs.emplace(
                    position,
                    InstrumentIdEventPair{instrument_id,
                                          StartNoteEvent{pitch, intensity}});
                // Perform note off event.
                if (note_end_position < end_position) {
                  id_event_pairs.emplace(
                      note_end_position,
                      InstrumentIdEventPair{instrument_id,
                                            StopNoteEvent{pitch}});
                } else {
                  performer.active_notes.emplace(
                      position, ActiveNote{note_end_position, pitch});
                }
              });
        }
        for (const auto& [position, id_event_pair] : id_event_pairs) {
          const auto& [instrument_id, event] = id_event_pair;
          if (auto* controller = FindOrNull(controllers_, instrument_id)) {
            controller->ProcessEvent(event, transport_.GetTimestamp(position));
          }
        }
      }) {}

Id Engine::AddPerformer() noexcept {
  const Id performer_id = id_generator_.Next();
  performers_.emplace(performer_id, Performer{});
  return performer_id;
}

StatusOr<Id> Engine::AddPerformerNote(Id performer_id, double position,
                                      Note note) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    const Id note_id = id_generator_.Next();
    performer->sequence.AddNote(note_id, position, note);
    return note_id;
  }
  return Status::kNotFound;
}

Id Engine::CreateInstrument(InstrumentDefinition definition,
                            int sample_rate) noexcept {
  const Id instrument_id = id_generator_.Next();
  controllers_.emplace(instrument_id,
                       InstrumentController{definition, &NoopNoteOffCallback,
                                            &NoopNoteOnCallback});
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
    for (auto& [performer_id, performer] : performers_) {
      if (performer.instrument_id == instrument_id) {
        performer.instrument_id = kInvalidId;
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

StatusOr<Param> Engine::GetInstrumentParam(Id instrument_id,
                                           int index) const noexcept {
  if (const auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (const auto* param = controller->GetParam(index)) {
      return *param;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerBeginPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetBeginPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerEndPosition(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetEndPosition();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopBeginOffset(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetLoopBeginOffset();
  }
  return Status::kNotFound;
}

StatusOr<double> Engine::GetPerformerLoopLength(
    Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.GetLoopLength();
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

StatusOr<bool> Engine::IsPerformerEmpty(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.IsEmpty();
  }
  return Status::kNotFound;
}

StatusOr<bool> Engine::IsPerformerLooping(Id performer_id) const noexcept {
  if (const auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.IsLooping();
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

Status Engine::RemoveAllPerformerNotes(Id performer_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.RemoveAllNotes();
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemoveAllPerformerNotes(Id performer_id, double begin_position,
                                       double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.RemoveAllNotes(begin_position, end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformer(Id performer_id) noexcept {
  if (const auto performer_it = performers_.find(performer_id);
      performer_it != performers_.end()) {
    const double timestamp = transport_.GetTimestamp();
    const auto instrument_id = performer_it->second.instrument_id;
    if (auto* controller = FindOrNull(controllers_, instrument_id)) {
      for (auto& [position, active_note] : performer_it->second.active_notes) {
        controller->ProcessEvent(StopNoteEvent{active_note.pitch}, timestamp);
      }
    }
    performers_.erase(performer_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::RemovePerformerNote(Id performer_id, Id note_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return performer->sequence.RemoveNote(note_id);
  }
  return Status::kNotFound;
}

Status Engine::ResetAllInstrumentParams(Id instrument_id) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->ResetAllParams(transport_.GetTimestamp());
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::ResetInstrumentParam(Id instrument_id, int index) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->ResetParam(index, transport_.GetTimestamp())) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

void Engine::SetConductor(ConductorDefinition definition) noexcept {
  conductor_ = Conductor{std::move(definition)};
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
    Id instrument_id, NoteOffCallback note_off_callback) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOffCallback(note_off_callback
                                       ? std::move(note_off_callback)
                                       : &NoopNoteOffCallback);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentNoteOnCallback(
    Id instrument_id, NoteOnCallback note_on_callback) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    controller->SetNoteOnCallback(note_on_callback ? std::move(note_on_callback)
                                                   : &NoopNoteOnCallback);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetInstrumentParam(Id instrument_id, int index,
                                  float value) noexcept {
  if (auto* controller = FindOrNull(controllers_, instrument_id)) {
    if (controller->SetParam(index, value, transport_.GetTimestamp())) {
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerBeginOffset(Id performer_id,
                                       double begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetBeginOffset(begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerBeginPosition(Id performer_id,
                                         double begin_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetBeginPosition(begin_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerEndPosition(Id performer_id,
                                       double end_position) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetEndPosition(end_position);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoop(Id performer_id, bool loop) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoop(loop);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerInstrument(Id performer_id,
                                      Id instrument_id) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    if (performer->instrument_id != instrument_id) {
      if (auto* controller =
              FindOrNull(controllers_, performer->instrument_id)) {
        for (auto& [position, active_note] : performer->active_notes) {
          controller->ProcessEvent(StopNoteEvent{active_note.pitch},
                                   transport_.GetTimestamp());
        }
      }
      performer->instrument_id = instrument_id;
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopBeginOffset(Id performer_id,
                                           double loop_begin_offset) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoopBeginOffset(loop_begin_offset);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetPerformerLoopLength(Id performer_id,
                                      double loop_length) noexcept {
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    performer->sequence.SetLoopLength(loop_length);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetPlaybackBeatCallback(BeatCallback beat_callback) noexcept {
  transport_.SetBeatCallback(beat_callback ? std::move(beat_callback)
                                           : &NoopBeatCallback);
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
  for (auto& [performer_id, performer] : performers_) {
    performer.active_notes.clear();
  }
  transport_.Stop();
  for (auto& [instrument_id, controller] : controllers_) {
    controller.StopAllNotes(transport_.GetTimestamp());
  }
}

void Engine::Update(double timestamp) noexcept {
  transport_.SetTempo(conductor_.TransformPlaybackTempo(playback_tempo_) *
                      kMinutesFromSeconds);
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
