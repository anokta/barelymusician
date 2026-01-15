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
    for (uint32_t task_index = GetNextInactiveTask(performer); task_index != UINT32_MAX;
         task_index = GetNextInactiveTask(performer)) {
      const auto& task = engine_.GetTask(task_index);
      if (!task.IsInside(performer.position) ||
          (task.position >= performer.position && task.priority > max_priority)) {
        break;
      }
      SetTaskActive(performer, task_index, true);
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

  uint32_t task_index = performer.first_active_task_index;
  while (task_index != UINT32_MAX) {
    auto& task = engine_.GetTask(task_index);
    task.is_active = false;
    task.callback(BarelyTaskEventType_kEnd);
    engine_.task_pool.Release(task_index);
    task_index = task.next_task_index;
  }
  performer.first_active_task_index = UINT32_MAX;

  task_index = performer.first_inactive_task_index;
  while (task_index != UINT32_MAX) {
    engine_.task_pool.Release(task_index);
    task_index = engine_.GetTask(task_index).next_task_index;
  }
  performer.first_inactive_task_index = UINT32_MAX;

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
    InsertInactiveTask(engine_.GetPerformer(performer_index), task_index);
  }
  return task_index;
}

void PerformerController::ReleaseTask(uint32_t task_index) noexcept {
  auto& task = engine_.GetTask(task_index);
  RemoveTask(engine_.GetPerformer(task.performer_index), task_index);
  if (task.is_active) {
    task.is_active = false;
    task.callback(BarelyTaskEventType_kEnd);
  }
}

void PerformerController::GetNextTaskEvent(const PerformerState& performer, double& duration,
                                           int32_t& priority) const noexcept {
  if (!performer.is_playing) {
    return;
  }

  const double loop_end_position = performer.GetLoopEndPosition();

  // Check inactive tasks.
  uint32_t task_index = performer.first_inactive_task_index;
  while (task_index != UINT32_MAX) {
    const auto& task = engine_.GetTask(task_index);
    if (task.position < performer.position ||
        (task.position == performer.position && task.priority <= priority)) {
      // If the performer position is inside an inactive task, we can return immediately.
      if (task.GetEndPosition() > performer.position) {
        duration = 0.0;
        priority = std::min(task.priority, priority);
        return;
      } else if (performer.is_looping && task.position >= performer.loop_begin_position &&
                 task.position < loop_end_position) {
        const double looped_inactive_duration =
            task.position - performer.position + performer.loop_length;
        if (looped_inactive_duration < duration ||
            (looped_inactive_duration == duration && task.priority < priority)) {
          duration = looped_inactive_duration;
          priority = task.priority;
        }
      }
    } else {
      const double inactive_duration = task.position - performer.position;
      if (inactive_duration < duration ||
          (inactive_duration == duration && task.priority < priority)) {
        duration = inactive_duration;
        priority = task.priority;
      }
      break;
    }
    task_index = task.next_task_index;
  }

  // Check active tasks.
  if (performer.first_active_task_index != UINT32_MAX) {
    const auto& active_task = engine_.GetTask(performer.first_active_task_index);
    const double end_position = performer.is_looping
                                    ? std::min(active_task.GetEndPosition(), loop_end_position)
                                    : active_task.GetEndPosition();
    const double active_duration = end_position - performer.position;
    if (active_duration < duration ||
        (active_duration == duration && active_task.priority < priority)) {
      duration = active_duration;
      priority = active_task.priority;
    }
  }
}

}  // namespace barely
