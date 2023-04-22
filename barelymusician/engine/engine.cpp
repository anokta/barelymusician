
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <functional>
#include <utility>

#include "barelymusician/barelymusician.h"
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
  [[maybe_unused]] const auto success =
      instruments_
          .emplace(instrument_id,
                   std::make_unique<Instrument>(definition, frame_rate, tempo_,
                                                timestamp_))
          .second;
  assert(success);
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

StatusOr<Id> Engine::CreateInstrumentEffect(Id instrument_id,
                                            EffectDefinition definition,
                                            int process_order) noexcept {
  if (instrument_id == kInvalid) {
    return Status::InvalidArgument();
  }
  auto instrument_or = GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    auto& instrument = instrument_or->get();
    const Id effect_id = GenerateNextId();
    instrument.CreateEffect(effect_id, definition, process_order);
    return effect_id;
  }
  return instrument_or.GetErrorStatus();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreatePerformer() noexcept {
  const Id performer_id = GenerateNextId();
  [[maybe_unused]] const auto success =
      performers_.emplace(performer_id, Performer{}).second;
  assert(success);
  return performer_id;
}

StatusOr<Id> Engine::CreatePerformerTask(Id performer_id,
                                         TaskDefinition definition,
                                         bool is_one_off, double position,
                                         int process_order,
                                         void* user_data) noexcept {
  if (performer_id == kInvalid) {
    return Status::InvalidArgument();
  }
  auto performer_or = GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    auto& performer = performer_or->get();
    if (is_one_off && position < performer.GetPosition()) {
      return Status::InvalidArgument();
    }
    const Id task_id = GenerateNextId();
    performer.CreateTask(task_id, definition, is_one_off, position,
                         process_order, user_data);
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
  if (instrument_id == kInvalid) {
    return Status::InvalidArgument();
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    return (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
  }
  return Status::NotFound();
}

void Engine::SetTempo(double tempo) noexcept {
  tempo = std::max(tempo, 0.0);
  if (tempo_ == tempo) {
    return;
  }
  tempo_ = tempo;
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetTempo(tempo_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      std::pair<double, int> update_duration = {
          BeatsFromSeconds(tempo_, timestamp - timestamp_),
          std::numeric_limits<int>::max()};
      bool has_tasks_to_process = false;
      for (const auto& [performer_id, performer] : performers_) {
        if (const auto maybe_duration = performer.GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }

      if (update_duration.first > 0) {
        for (auto& [performer_id, performer] : performers_) {
          performer.Update(update_duration.first);
        }

        timestamp_ += SecondsFromBeats(tempo_, update_duration.first);
        for (auto& [instrument_id, instrument] : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (auto& [performer_id, performer] : performers_) {
          performer.ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (auto& [instrument_id, instrument] : instruments_) {
        instrument->Update(timestamp_);
      }
    }
  }
}

Id Engine::GenerateNextId() noexcept {
  const Id id = ++id_counter_;
  assert(id > kInvalid);
  return id;
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
