#include "engine/performer_controller.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

uint32_t PerformerController::Acquire() noexcept {
  const uint32_t performer_index = engine_.performer_pool.Acquire();
  if (performer_index != UINT32_MAX) {
    auto& performer = engine_.GetPerformer(performer_index);
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
    const uint32_t next_task_index = task.next_task_index;
    engine_.task_pool.Release(task_index);
    task_index = next_task_index;
  }
  performer.first_active_task_index = UINT32_MAX;

  task_index = performer.first_inactive_task_index;
  while (task_index != UINT32_MAX) {
    const uint32_t next_task_index = engine_.GetTask(task_index).next_task_index;
    engine_.task_pool.Release(task_index);
    task_index = next_task_index;
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
  engine_.task_pool.Release(task_index);
}

void PerformerController::SetLoopBeginPosition(uint32_t performer_index,
                                               double loop_begin_position) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  if (performer.loop_begin_position == loop_begin_position) {
    return;
  }
  performer.loop_begin_position = loop_begin_position;
  if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
    SetPosition(performer_index, performer.LoopAround(performer.position));
  }
}

void PerformerController::SetLoopLength(uint32_t performer_index, double loop_length) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  if (performer.loop_length == loop_length) {
    return;
  }
  performer.loop_length = loop_length;
  if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
    SetPosition(performer_index, performer.LoopAround(performer.position));
  }
}

void PerformerController::SetLooping(uint32_t performer_index, bool is_looping) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  if (performer.is_looping == is_looping) {
    return;
  }
  performer.is_looping = is_looping;
  if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
    SetPosition(performer_index, performer.LoopAround(performer.position));
  }
}

void PerformerController::SetPosition(uint32_t performer_index, double position) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  if (performer.position == position) {
    return;
  }
  if (performer.is_looping && position >= performer.GetLoopEndPosition()) {
    performer.position = performer.LoopAround(position);
    while (performer.first_active_task_index != UINT32_MAX) {
      SetTaskActive(performer, performer.first_active_task_index, false);
    }
  } else {
    performer.position = position;
    uint32_t task_index = performer.first_active_task_index;
    while (task_index != UINT32_MAX) {
      auto& task = engine_.GetTask(task_index);
      if (task.IsInside(performer.position)) {
        task_index = task.next_task_index;
      } else {
        SetTaskActive(performer, task_index, false);
        // Restart the iteration since links can get invalidated after a callback.
        task_index = performer.first_active_task_index;
      }
    }
  }
}

void PerformerController::Start(uint32_t performer_index) noexcept {
  engine_.GetPerformer(performer_index).is_playing = true;
}

void PerformerController::Stop(uint32_t performer_index) noexcept {
  auto& performer = engine_.GetPerformer(performer_index);
  performer.is_playing = false;
  while (performer.first_active_task_index != UINT32_MAX) {
    SetTaskActive(performer, performer.first_active_task_index, false);
  }
}

void PerformerController::SetTaskDuration(uint32_t task_index, double duration) noexcept {
  assert(duration > 0.0 && "Invalid task duration");
  auto& task = engine_.GetTask(task_index);
  auto& performer = engine_.GetPerformer(task.performer_index);
  if (task.duration == duration) return;
  task.duration = duration;
  if (task.is_active) {
    if (task.IsInside(performer.position)) {
      RemoveTask(performer, task_index);
      InsertActiveTask(performer, task_index);
    } else {
      SetTaskActive(performer, task_index, false);
    }
  }
}

void PerformerController::SetTaskEventCallback(uint32_t task_index,
                                               BarelyTaskEventCallback callback,
                                               void* user_data) noexcept {
  engine_.GetTask(task_index).SetEventCallback(callback, user_data);
}

void PerformerController::SetTaskPosition(uint32_t task_index, double position) noexcept {
  auto& task = engine_.GetTask(task_index);
  auto& performer = engine_.GetPerformer(task.performer_index);
  if (task.position == position) return;
  task.position = position;
  if (task.is_active) {
    if (task.IsInside(performer.position)) {
      RemoveTask(performer, task_index);
      InsertActiveTask(performer, task_index);
    } else {
      SetTaskActive(performer, task_index, false);
    }
  } else {
    RemoveTask(performer, task_index);
    InsertInactiveTask(performer, task_index);
  }
}

void PerformerController::SetTaskPriority(uint32_t task_index, int32_t priority) noexcept {
  auto& task = engine_.GetTask(task_index);
  auto& performer = engine_.GetPerformer(task.performer_index);
  if (task.priority == priority) return;
  task.priority = priority;
  if (task.is_active) {
    RemoveTask(performer, task_index);
    InsertActiveTask(performer, task_index);
  } else {
    RemoveTask(performer, task_index);
    InsertInactiveTask(performer, task_index);
  }
}

void PerformerController::ProcessAllTasksAtPosition(int32_t max_priority) noexcept {
  for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
    auto& performer = engine_.performer_pool.GetActive(i);
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
    auto& performer = engine_.performer_pool.GetActive(i);
    if (!performer.is_playing) {
      continue;
    }
    SetPosition(engine_.performer_pool.GetIndex(performer), performer.position + duration);
  }
}

void PerformerController::InsertActiveTask(PerformerState& performer,
                                           uint32_t task_index) noexcept {
  auto& task = engine_.GetTask(task_index);
  if (performer.first_active_task_index == UINT32_MAX) {
    performer.first_active_task_index = task_index;
    task.prev_task_index = UINT32_MAX;
    task.next_task_index = UINT32_MAX;
  } else if (auto& first_active_task = engine_.GetTask(performer.first_active_task_index);
             task.IsActiveBefore(first_active_task)) {
    first_active_task.prev_task_index = task_index;
    task.next_task_index = performer.first_active_task_index;
    performer.first_active_task_index = task_index;
  } else {
    uint32_t active_task_index = performer.first_active_task_index;
    while (active_task_index != UINT32_MAX) {
      const auto& active_task = engine_.GetTask(active_task_index);
      if (active_task.next_task_index == UINT32_MAX ||
          task.IsActiveBefore(engine_.GetTask(active_task.next_task_index))) {
        break;
      }
      active_task_index = active_task.next_task_index;
    }
    auto& active_task = engine_.GetTask(active_task_index);
    if (active_task.next_task_index != UINT32_MAX) {
      engine_.GetTask(active_task.next_task_index).prev_task_index = task_index;
    }
    task.next_task_index = active_task.next_task_index;
    active_task.next_task_index = task_index;
    task.prev_task_index = active_task_index;
  }
}

void PerformerController::InsertInactiveTask(PerformerState& performer,
                                             uint32_t task_index) noexcept {
  auto& task = engine_.GetTask(task_index);
  if (performer.first_inactive_task_index == UINT32_MAX) {
    performer.first_inactive_task_index = task_index;
    task.prev_task_index = UINT32_MAX;
    task.next_task_index = UINT32_MAX;
  } else if (auto& first_inactive_task = engine_.GetTask(performer.first_inactive_task_index);
             task.IsInactiveBefore(first_inactive_task)) {
    first_inactive_task.prev_task_index = task_index;
    task.next_task_index = performer.first_inactive_task_index;
    performer.first_inactive_task_index = task_index;
  } else {
    uint32_t inactive_task_index = performer.first_inactive_task_index;
    while (inactive_task_index != UINT32_MAX) {
      const auto& inactive_task = engine_.GetTask(inactive_task_index);
      if (inactive_task.next_task_index == UINT32_MAX ||
          task.IsInactiveBefore(engine_.GetTask(inactive_task.next_task_index))) {
        break;
      }
      inactive_task_index = inactive_task.next_task_index;
    }
    auto& inactive_task = engine_.GetTask(inactive_task_index);
    if (inactive_task.next_task_index != UINT32_MAX) {
      engine_.GetTask(inactive_task.next_task_index).prev_task_index = task_index;
    }
    task.next_task_index = inactive_task.next_task_index;
    inactive_task.next_task_index = task_index;
    task.prev_task_index = inactive_task_index;
  }
}

void PerformerController::RemoveTask(PerformerState& performer, uint32_t task_index) noexcept {
  auto& task = engine_.GetTask(task_index);
  if (task.prev_task_index == UINT32_MAX) {
    (task.is_active ? performer.first_active_task_index : performer.first_inactive_task_index) =
        task.next_task_index;
  } else {
    engine_.GetTask(task.prev_task_index).next_task_index = task.next_task_index;
  }
  if (task.next_task_index != UINT32_MAX) {
    engine_.GetTask(task.next_task_index).prev_task_index = task.prev_task_index;
  }
  task.prev_task_index = UINT32_MAX;
  task.next_task_index = UINT32_MAX;
}

void PerformerController::SetTaskActive(PerformerState& performer, uint32_t task_index,
                                        bool is_active) noexcept {
  auto& task = engine_.GetTask(task_index);
  assert(task.is_active != is_active);

  RemoveTask(performer, task_index);
  task.is_active = is_active;

  if (is_active) {
    InsertActiveTask(performer, task_index);
    task.callback(BarelyTaskEventType_kBegin);
  } else {
    InsertInactiveTask(performer, task_index);
    task.callback(BarelyTaskEventType_kEnd);
  }
}

uint32_t PerformerController::GetNextInactiveTask(const PerformerState& performer) const noexcept {
  if (!performer.is_playing) {
    return UINT32_MAX;
  }
  uint32_t task_index = performer.first_inactive_task_index;
  while (task_index != UINT32_MAX) {
    const auto& task = engine_.GetTask(task_index);
    if (task.position >= performer.position || task.GetEndPosition() > performer.position) {
      return task_index;
    }
    task_index = task.next_task_index;
  }
  return UINT32_MAX;
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
      }
      if (performer.is_looping && task.position >= performer.loop_begin_position &&
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
