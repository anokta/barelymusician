
#include "barelymusician/internal/engine.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/seconds.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetAllNotesOff();
  }
  instrument_refs_.Update({});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::optional<Id> Engine::CreateInstrument(InstrumentDefinition definition,
                                           int frame_rate) noexcept {
  if (frame_rate <= 0) {
    return std::nullopt;
  }
  const Id instrument_id = GenerateNextId();
  [[maybe_unused]] const auto success =
      instruments_
          .emplace(instrument_id,
                   std::make_unique<Instrument>(definition, frame_rate, tempo_, timestamp_))
          .second;
  assert(success);
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

std::optional<Id> Engine::CreateInstrumentEffect(Id instrument_id, EffectDefinition definition,
                                                 int process_order) noexcept {
  if (instrument_id == kInvalid) {
    return std::nullopt;
  }
  auto instrument_or = GetInstrument(instrument_id);
  if (instrument_or.has_value()) {
    auto& instrument = instrument_or->get();
    const Id effect_id = GenerateNextId();
    instrument.CreateEffect(effect_id, definition, process_order);
    return effect_id;
  }
  return std::nullopt;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
std::shared_ptr<Performer> Engine::CreatePerformer() noexcept {
  auto performer = std::shared_ptr<Performer>(new (std::nothrow) Performer());
  [[maybe_unused]] const auto success = performers_.emplace(performer.get()).second;
  assert(success);
  return performer;
}

std::optional<Id> Engine::CreatePerformerTask(Performer* performer, TaskDefinition definition,
                                              bool is_one_off, double position, int process_order,
                                              void* user_data) noexcept {
  if (performer == nullptr) {
    return std::nullopt;
  }
  if (is_one_off && position < performer->GetPosition()) {
    return std::nullopt;
  }
  const Id task_id = GenerateNextId();
  performer->CreateTask(task_id, definition, is_one_off, position, process_order, user_data);
  return task_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) {
    return false;
  }
  if (const auto it = instruments_.find(instrument_id); it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff();
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::DestroyPerformer(Performer* performer) noexcept {
  [[maybe_unused]] const auto success = performers_.erase(performer) > 0;
  assert(success);
}

std::optional<std::reference_wrapper<Instrument>> Engine::GetInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) {
    return std::nullopt;
  }
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return std::ref(*(*instrument));
  }
  return std::nullopt;
}

double Engine::GetTempo() const noexcept { return tempo_; }

double Engine::GetTimestamp() const noexcept { return timestamp_; }

bool Engine::ProcessInstrument(Id instrument_id, double* output_samples, int output_channel_count,
                               int output_frame_count, double timestamp) noexcept {
  if (instrument_id == kInvalid) {
    return false;
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref = FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    return (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count, timestamp);
  }
  return false;
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
      std::pair<double, int> update_duration = {BeatsFromSeconds(tempo_, timestamp - timestamp_),
                                                std::numeric_limits<int>::max()};
      bool has_tasks_to_process = false;
      for (const auto* performer : performers_) {
        if (const auto maybe_duration = performer->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }

      if (update_duration.first > 0) {
        for (auto* performer : performers_) {
          performer->Update(update_duration.first);
        }

        timestamp_ += SecondsFromBeats(tempo_, update_duration.first);
        for (auto& [instrument_id, instrument] : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (auto* performer : performers_) {
          performer->ProcessNextTaskAtPosition();
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
