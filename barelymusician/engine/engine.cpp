
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <functional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/status.h"

namespace barely::internal {

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetAllNotesOff();
  }
  instrument_refs_.Update({});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreateInstrument(InstrumentDefinition definition,
                                      int frame_rate) noexcept {
  if (frame_rate < 0) return Status::InvalidArgumentError();
  const Id instrument_id = GenerateNextId();
  const auto success = instruments_
                           .emplace(instrument_id, std::make_unique<Instrument>(
                                                       definition, frame_rate,
                                                       tempo_, timestamp_))
                           .second;
  assert(success);
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreatePerformer() noexcept {
  const Id performer_id = GenerateNextId();
  const auto success = performers_.emplace(performer_id, Performer{}).second;
  assert(success);
  return performer_id;
}

StatusOr<Id> Engine::CreatePerformerTask(Id performer_id,
                                         TaskDefinition definition,
                                         double position, TaskType type,
                                         int order, void* user_data) noexcept {
  if (performer_id == kInvalid) return Status::InvalidArgumentError();
  if (position < 0.0) return Status::InvalidArgumentError();
  auto performer_or = GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    const Id task_id = GenerateNextId();
    performer_or->get().CreateTask(task_id, definition, position, type, order,
                                   user_data);
    return task_id;
  }
  return performer_or.GetErrorStatus();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return Status::InvalidArgumentError();
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff();
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return Status::OkStatus();
  }
  return Status::NotFoundError();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyPerformer(Id performer_id) noexcept {
  if (performer_id == kInvalid) return Status::InvalidArgumentError();
  if (performers_.erase(performer_id) > 0) {
    return Status::OkStatus();
  }
  return Status::NotFoundError();
}

StatusOr<std::reference_wrapper<Instrument>> Engine::GetInstrument(
    Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return Status::InvalidArgumentError();
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return std::ref(*(*instrument));
  }
  return Status::NotFoundError();
}

StatusOr<std::reference_wrapper<Performer>> Engine::GetPerformer(
    [[maybe_unused]] Id performer_id) noexcept {
  if (performer_id == kInvalid) return Status::InvalidArgumentError();
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return std::ref(*performer);
  }
  return Status::NotFoundError();
}

double Engine::GetTempo() const noexcept { return tempo_; }

double Engine::GetTimestamp() const noexcept { return timestamp_; }

Status Engine::ProcessInstrument(Id instrument_id, double* output_samples,
                                 int output_channel_count,
                                 int output_frame_count,
                                 double timestamp) noexcept {
  if (instrument_id == kInvalid) return Status::InvalidArgumentError();
  if ((!output_samples && output_channel_count > 0 && output_frame_count) ||
      output_channel_count < 0 || output_frame_count < 0 || timestamp < 0.0) {
    return Status::InvalidArgumentError();
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
    return Status::OkStatus();
  }
  return Status::NotFoundError();
}

void Engine::SetTempo(double tempo) noexcept {
  tempo = std::max(tempo, 0.0);
  if (tempo_ == tempo) return;
  tempo_ = tempo;
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetTempo(tempo_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);

  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      double update_duration = BeatsFromSeconds(tempo_, timestamp - timestamp_);

      bool has_tasks_to_process = false;
      for (const auto& [performer_id, performer] : performers_) {
        if (const auto maybe_duration = performer.GetDurationToNextTask();
            maybe_duration && *maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }

      for (auto& [performer_id, performer] : performers_) {
        performer.Update(update_duration);
      }

      timestamp_ += SecondsFromBeats(tempo_, update_duration);
      for (auto& [instrument_id, instrument] : instruments_) {
        instrument->Update(timestamp_);
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (auto& [performer_id, performer] : performers_) {
          performer.ProcessAllTasksAtCurrentPosition();
        }
      }
    } else {
      timestamp_ = timestamp;
      for (auto& [instrument_id, instrument] : instruments_) {
        instrument->Update(timestamp_);
      }
    }
  }
}

Id Engine::GenerateNextId() noexcept {
  assert(id_counter_ >= 0);
  return ++id_counter_;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::UpdateInstrumentReferenceMap() noexcept {
  InstrumentReferenceMap new_instrument_refs;
  new_instrument_refs.reserve(instruments_.size());
  for (const auto& [instrument_id, instrument] : instruments_) {
    new_instrument_refs.emplace(instrument_id, instrument.get());
  }
  instrument_refs_.Update(std::move(new_instrument_refs));
}

}  // namespace barely::internal
