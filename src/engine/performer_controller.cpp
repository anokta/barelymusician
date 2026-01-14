#include "engine/performer_controller.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

void PerformerController::ProcessAllTasksAtPosition(int32_t max_priority) noexcept {
  for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
    PerformerState& performer = engine_.performer_pool.GetActive(i);
    if (!performer.is_playing) {
      continue;
    }
    // Active tasks get processed in `SetPosition`, so we only need to process inactive tasks.
    for (auto it = GetNextInactiveTask(performer);
         it != performer.inactive_tasks.end() &&
         engine_.GetTask(it->second).IsInside(performer.position) &&
         (it->first.first < performer.position || it->first.second <= max_priority);
         it = GetNextInactiveTask(performer)) {
      SetTaskActive(performer, it, true);
    }
  }
}

void PerformerController::Update(double duration) noexcept {
  assert(duration > 0.0);
  for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
    PerformerState& performer = engine_.performer_pool.GetActive(i);
    if (!performer.is_playing) {
      continue;
    }
    SetPosition(engine_.performer_pool.GetIndex(performer), performer.position + duration);
  }
}

uint32_t PerformerController::Acquire() noexcept {
  const uint32_t performer_index = engine_.performer_pool.Acquire();
  if (performer_index != UINT32_MAX) {
    PerformerState& performer = engine_.GetPerformer(performer_index);
    performer = {};
  }
  return performer_index;
}

void PerformerController::Release(uint32_t performer_index) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  for (const auto& [_, task_index] : performer.active_tasks) {
    engine_.GetTask(task_index).SetActive(false);
    engine_.task_pool.Release(task_index);
  }
  for (const auto& [_, task_index] : performer.inactive_tasks) {
    engine_.task_pool.Release(task_index);
  }
  engine_.performer_pool.Release(performer_index);
}

uint32_t PerformerController::AcquireTask(uint32_t performer_index, double position,
                                          double duration, int32_t priority,
                                          BarelyTaskEventCallback callback,
                                          void* user_data) noexcept {
  const uint32_t task_index = engine_.task_pool.Acquire();
  if (task_index != UINT32_MAX) {
    TaskState& task = engine_.GetTask(task_index);
    task = {{callback, user_data}, position, duration, priority, performer_index};
    [[maybe_unused]] const bool success =
        engine_.GetPerformer(performer_index)
            .inactive_tasks.emplace(TaskKey{position, priority}, task_index)
            .second;
    assert(success && "Failed to acquire task");
  }
  return task_index;
}

void PerformerController::ReleaseTask(uint32_t task_index) noexcept {
  auto& task = engine_.GetTask(task_index);
  auto& performer = engine_.GetPerformer(task.performer_index);
  if (task.is_active) {
    [[maybe_unused]] const bool success =
        (performer.active_tasks.erase({{task.GetEndPosition(), task.priority}, task_index}) == 1);
    assert(success && "Failed to destroy active task");
    task.SetActive(false);
  } else {
    [[maybe_unused]] const bool success =
        (performer.inactive_tasks.erase({{task.position, task.priority}, task_index}) == 1);
    assert(success && "Failed to destroy inactive task");
  }
  engine_.task_pool.Release(task_index);
}

TaskKey PerformerController::GetNextTaskKey(double duration) const noexcept {
  TaskKey next_key = {duration, INT32_MIN};
  for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
    UpdateNextKey(engine_.performer_pool.GetActive(i), next_key);
  }
  return next_key;
}

void PerformerController::UpdateNextKey(const PerformerState& performer,
                                        TaskKey& next_key) const noexcept {
  if (!performer.is_playing) {
    return;
  }

  const double loop_end_position = performer.GetLoopEndPosition();

  // Check inactive tasks.
  if (!performer.inactive_tasks.empty()) {
    const auto next_it = performer.inactive_tasks.lower_bound({{performer.position, INT32_MIN}, 0});
    // If the performer position is inside an inactive task, we can return immediately.
    // TODO(#147): This may be optimized further using an interval tree.
    for (auto it = performer.inactive_tasks.begin(); it != next_it; ++it) {
      if (engine_.GetTask(it->second).GetEndPosition() > performer.position) {
        next_key = {0.0, std::min(next_key.second, it->first.second)};
        return;
      }
    }
    // Loop around if needed.
    if (performer.is_looping &&
        (next_it == performer.inactive_tasks.end() || next_it->first.first >= loop_end_position)) {
      if (const auto loop_it =
              performer.inactive_tasks.lower_bound({{performer.loop_begin_position, INT32_MIN}, 0});
          loop_it != performer.inactive_tasks.end() && loop_it->first.first < loop_end_position) {
        next_key = std::min({loop_it->first.first + performer.loop_length - performer.position,
                             loop_it->first.second},
                            next_key);
      }
    } else if (next_it != performer.inactive_tasks.end()) {
      next_key = std::min({next_it->first.first - performer.position, next_it->second}, next_key);
    }
  }

  // Check active tasks.
  if (!performer.active_tasks.empty()) {
    const TaskKey next_active_key =
        performer.is_looping
            ? TaskKey{std::min(performer.active_tasks.begin()->first.first, loop_end_position),
                      performer.active_tasks.begin()->first.second}
            : performer.active_tasks.begin()->first;
    next_key =
        std::min({next_active_key.first - performer.position, next_active_key.second}, next_key);
  }
}

}  // namespace barely
