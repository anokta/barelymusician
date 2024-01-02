
#include "barelymusician/internal/engine.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/seconds.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Observer<Instrument> Engine::CreateInstrument(InstrumentDefinition definition,
                                              int frame_rate) noexcept {
  Observable<Instrument> observable(definition, frame_rate, tempo_, timestamp_);
  const auto [it, success] = instruments_.emplace(observable.get(), std::move(observable));
  assert(success);
  return it->second.Observe();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Observer<Performer> Engine::CreatePerformer() noexcept {
  Observable<Performer> observable;
  const auto [it, success] = performers_.emplace(observable.get(), std::move(observable));
  assert(success);
  return it->second.Observe();
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
void Engine::DestroyInstrument(Instrument& instrument) noexcept {
  [[maybe_unused]] const auto success = instruments_.erase(&instrument) > 0;
  assert(success);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::DestroyPerformer(Performer& performer) noexcept {
  [[maybe_unused]] const auto success = performers_.erase(&performer) > 0;
  assert(success);
}

double Engine::GetTempo() const noexcept { return tempo_; }

double Engine::GetTimestamp() const noexcept { return timestamp_; }

void Engine::SetTempo(double tempo) noexcept {
  tempo = std::max(tempo, 0.0);
  if (tempo_ == tempo) {
    return;
  }
  tempo_ = tempo;
  for (auto& [instrument, observable] : instruments_) {
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
      for (const auto& [performer, observable] : performers_) {
        if (const auto maybe_duration = performer->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }

      if (update_duration.first > 0) {
        for (auto& [performer, observable] : performers_) {
          performer->Update(update_duration.first);
        }

        timestamp_ += SecondsFromBeats(tempo_, update_duration.first);
        for (auto& [instrument, observable] : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (auto& [performer, observable] : performers_) {
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (auto& [instrument, observable] : instruments_) {
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

}  // namespace barely::internal
