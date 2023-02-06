
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/number.h"
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
                                      Integer frame_rate) noexcept {
  if (frame_rate < 0) return {Status::kInvalidArgument};
  const Id instrument_id = GenerateNextId();
  const auto [it, success] = instruments_.emplace(
      instrument_id,
      std::make_unique<Instrument>(definition, frame_rate, tempo_, timestamp_));
  assert(success);
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreatePerformer(Integer order) noexcept {
  const Id performer_id = GenerateNextId();
  auto [it, success] =
      performers_.emplace(std::pair{order, performer_id}, Performer{});
  assert(success);
  success = performer_refs_
                .emplace(performer_id, std::pair{order, std::ref(it->second)})
                .second;
  assert(success);
  return performer_id;
}

StatusOr<Id> Engine::CreatePerformerTask(Id performer_id,
                                         TaskDefinition definition,
                                         Real position, TaskType type,
                                         void* user_data) noexcept {
  if (performer_id == kInvalid) return {Status::kInvalidArgument};
  if (position < 0.0) return {Status::kInvalidArgument};
  auto performer_or = GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    const Id task_id = GenerateNextId();
    performer_or->get().CreateTask(task_id, definition, position, type,
                                   user_data);
    return task_id;
  }
  return performer_or.GetErrorStatus();
}

StatusOr<Id> Engine::CreateTask(TaskDefinition definition, Real timestamp,
                                void* user_data) noexcept {
  if (timestamp < timestamp_) return {Status::kInvalidArgument};
  const Id task_id = GenerateNextId();
  auto success = task_timestamps_.emplace(task_id, timestamp).second;
  assert(success);
  success = tasks_
                .emplace(std::pair{timestamp, task_id},
                         std::make_unique<Task>(definition, user_data))
                .second;
  assert(success);
  return task_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff();
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return Status::kOk;
  }
  return Status::kNotFound;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyPerformer(Id performer_id) noexcept {
  if (performer_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = performer_refs_.find(performer_id);
      it != performer_refs_.end()) {
    [[maybe_unused]] const bool success =
        performers_.erase(std::pair{it->second.first, performer_id}) > 0;
    assert(success);
    performer_refs_.erase(it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyTask(Id task_id) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = task_timestamps_.find(task_id);
      it != task_timestamps_.end()) {
    const auto success = tasks_.erase(std::pair{it->second, task_id}) == 1;
    assert(success);
    task_timestamps_.erase(it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::reference_wrapper<Instrument>> Engine::GetInstrument(
    Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return {Status::kInvalidArgument};
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return std::ref(*(*instrument));
  }
  return {Status::kNotFound};
}

StatusOr<std::reference_wrapper<Performer>> Engine::GetPerformer(
    [[maybe_unused]] Id performer_id) noexcept {
  if (performer_id == kInvalid) return {Status::kInvalidArgument};
  if (auto* performer_ref = FindOrNull(performer_refs_, performer_id)) {
    return performer_ref->second;
  }
  return {Status::kNotFound};
}

StatusOr<Real> Engine::GetTaskTimestamp(Id task_id) const noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto* task_timestamp = FindOrNull(task_timestamps_, task_id)) {
    return *task_timestamp;
  }
  return {Status::kNotFound};
}

Real Engine::GetTempo() const noexcept { return tempo_; }

Real Engine::GetTimestamp() const noexcept { return timestamp_; }

Status Engine::ProcessInstrument(Id instrument_id, Real* output_samples,
                                 Integer output_channel_count,
                                 Integer output_frame_count,
                                 Real timestamp) noexcept {
  if (instrument_id == kInvalid) return Status::kInvalidArgument;
  if ((!output_samples && output_channel_count > 0 && output_frame_count) ||
      output_channel_count < 0 || output_frame_count < 0 || timestamp < 0.0) {
    return Status::kInvalidArgument;
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Engine::SetTaskTimestamp(Id task_id, Real timestamp) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = task_timestamps_.find(task_id);
      it != task_timestamps_.end()) {
    auto& current_timestamp = it->second;
    if (current_timestamp != timestamp) {
      auto node = tasks_.extract(std::pair{current_timestamp, task_id});
      node.key().first = timestamp;
      tasks_.insert(std::move(node));
      current_timestamp = timestamp;
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetTempo(Real tempo) noexcept {
  tempo = std::max(tempo, 0.0);
  if (tempo_ == tempo) return;
  tempo_ = tempo;
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetTempo(tempo_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(Real timestamp) noexcept {
  assert(timestamp >= 0.0);

  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      Real update_duration = BeatsFromSeconds(tempo_, timestamp - timestamp_);
      std::unordered_set<Id> performer_ids_to_process;
      for (const auto& [order_id_pair, performer] : performers_) {
        if (const auto maybe_duration = performer.GetDurationToNextTask();
            maybe_duration && *maybe_duration <= update_duration) {
          if (*maybe_duration < update_duration) {
            update_duration = *maybe_duration;
            std::exchange(performer_ids_to_process, {order_id_pair.second});
          } else {
            performer_ids_to_process.emplace(order_id_pair.second);
          }
        }
      }

      Real next_timestamp =
          timestamp_ + SecondsFromBeats(tempo_, update_duration);

      bool has_tasks = false;
      if (!tasks_.empty() && tasks_.begin()->first.first <= next_timestamp) {
        has_tasks = true;
        if (tasks_.begin()->first.first < next_timestamp) {
          next_timestamp = tasks_.begin()->first.first;
          update_duration =
              BeatsFromSeconds(tempo_, next_timestamp - timestamp_);
          performer_ids_to_process.clear();
        }
      }

      for (auto& [order_id_pair, performer] : performers_) {
        if (performer_ids_to_process.find(order_id_pair.second) !=
            performer_ids_to_process.end()) {
          performer.UpdateToNextTask();
        } else {
          performer.Update(update_duration);
        }
      }
      if (next_timestamp > timestamp_) {
        timestamp_ = next_timestamp;
        for (auto& [instrument_id, instrument] : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks) {
        auto it = tasks_.begin();
        while (it != tasks_.end() && it->first.first <= next_timestamp) {
          it->second->Process();
          ++it;
        }
        tasks_.erase(tasks_.begin(), it);
      }

      if (!performer_ids_to_process.empty()) {
        for (auto& [order_id_pair, performer] : performers_) {
          if (performer_ids_to_process.find(order_id_pair.second) !=
              performer_ids_to_process.end()) {
            performer.ProcessAllTasksAtCurrentPosition();
          }
        }
      }
    } else {
      // Playback is stopped.
      auto it = tasks_.begin();
      while (it != tasks_.end() && it->first.first < timestamp) {
        timestamp_ = it->first.first;
        for (auto& [instrument_id, instrument] : instruments_) {
          instrument->Update(timestamp_);
        }
        it->second->Process();
        ++it;
      }
      tasks_.erase(tasks_.begin(), it);
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
