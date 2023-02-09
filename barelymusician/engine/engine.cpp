
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <functional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/seconds.h"
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
  if (frame_rate <= 0) {
    return Status::InvalidArgument();
  }
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
                                         double position, int process_order,
                                         void* user_data,
                                         bool is_one_off) noexcept {
  if (performer_id == kInvalid || position < 0.0) {
    return Status::InvalidArgument();
  }
  auto performer_or = GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    const Id task_id = GenerateNextId();
    performer_or->get().CreateTask(task_id, definition, position, process_order,
                                   user_data, is_one_off);
    return task_id;
  }
  return performer_or.GetErrorStatus();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) {
    return Status::InvalidArgument();
  }
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff();
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return Status::Ok();
  }
  return Status::NotFound();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyPerformer(Id performer_id) noexcept {
  if (performer_id == kInvalid) {
    return Status::InvalidArgument();
  }
  if (performers_.erase(performer_id) > 0) {
    return Status::Ok();
  }
  return Status::NotFound();
}

StatusOr<std::reference_wrapper<Instrument>> Engine::GetInstrument(
    Id instrument_id) noexcept {
  if (instrument_id == kInvalid) {
    return Status::InvalidArgument();
  }
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return std::ref(*(*instrument));
  }
  return Status::NotFound();
}

StatusOr<std::reference_wrapper<Performer>> Engine::GetPerformer(
    Id performer_id) noexcept {
  if (performer_id == kInvalid) {
    return Status::InvalidArgument();
  }
  if (auto* performer = FindOrNull(performers_, performer_id)) {
    return std::ref(*performer);
  }
  return Status::NotFound();
}

double Engine::GetTempo() const noexcept { return tempo_; }

double Engine::GetTimestamp() const noexcept { return timestamp_; }

Status Engine::ProcessInstrument(Id instrument_id, double* output_samples,
                                 int output_channel_count,
                                 int output_frame_count,
                                 double timestamp) noexcept {
  if (instrument_id == kInvalid ||
      (!output_samples && output_channel_count > 0 && output_frame_count > 0) ||
      output_channel_count < 0 || output_frame_count < 0 || timestamp < 0.0) {
    return Status::InvalidArgument();
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
    return Status::Ok();
  }
  return Status::NotFound();
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
      int order = std::numeric_limits<int>::max();
      for (const auto& [performer_id, performer] : performers_) {
        if (const auto maybe_duration = performer.GetDurationToNextTask();
            maybe_duration && (maybe_duration->first < update_duration ||
                               (maybe_duration->first == update_duration &&
                                maybe_duration->second < order))) {
          has_tasks_to_process = true;
          update_duration = maybe_duration->first;
          order = maybe_duration->second;
        }
      }

      if (update_duration > 0) {
        for (auto& [performer_id, performer] : performers_) {
          performer.Update(update_duration);
        }

        timestamp_ += SecondsFromBeats(tempo_, update_duration);
        for (auto& [instrument_id, instrument] : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (auto& [performer_id, performer] : performers_) {
          performer.ProcessNextTaskAtPosition();
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
