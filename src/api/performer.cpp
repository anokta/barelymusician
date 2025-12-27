#include "api/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <optional>
#include <set>
#include <utility>

#include "api/task.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::AddTask(BarelyTask* task) noexcept {
  [[maybe_unused]] const bool success =
      inactive_tasks_.emplace(TaskKey{task->position, task->priority}, task).second;
  assert(success && "Failed to create task");
}

std::optional<BarelyPerformer::TaskKey> BarelyPerformer::GetNextTaskKey() const noexcept {
  if (!is_playing) {
    return std::nullopt;
  }

  const double loop_end_position = GetLoopEndPosition();
  std::optional<TaskKey> next_key = std::nullopt;

  // Check inactive tasks.
  if (!inactive_tasks_.empty()) {
    const auto next_it =
        inactive_tasks_.lower_bound({{position, std::numeric_limits<int>::min()}, nullptr});
    // If the performer position is inside an inactive task, we can return immediately.
    // TODO(#147): This may be optimized further using an interval tree.
    for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
      if (it->second->GetEndPosition() > position) {
        return TaskKey{0.0, it->first.second};
      }
    }
    // Loop around if needed.
    if (is_looping &&
        (next_it == inactive_tasks_.end() || next_it->first.first >= GetLoopEndPosition())) {
      if (const auto loop_it = inactive_tasks_.lower_bound(
              {{loop_begin_position, std::numeric_limits<int>::min()}, nullptr});
          loop_it != inactive_tasks_.end() && loop_it->first.first < GetLoopEndPosition()) {
        next_key = {loop_it->first.first + loop_length, loop_it->first.second};
      }
    } else if (next_it != inactive_tasks_.end()) {
      next_key = next_it->first;
    }
  }

  // Check active tasks.
  if (!active_tasks_.empty()) {
    if (const TaskKey next_active_key =
            is_looping ? TaskKey{std::min(active_tasks_.begin()->first.first, loop_end_position),
                                 active_tasks_.begin()->first.second}
                       : active_tasks_.begin()->first;
        !next_key.has_value() || next_active_key < next_key) {
      next_key = next_active_key;
    }
  }

  if (next_key.has_value()) {
    assert(next_key->first >= position && "Invalid next duration");
    return TaskKey{next_key->first - position, next_key->second};
  }
  return std::nullopt;
}

void BarelyPerformer::ProcessAllTasksAtPosition(int max_priority) noexcept {
  if (!is_playing) {
    return;
  }
  // Active tasks get processed in `SetPosition`, so we only need to process inactive tasks here.
  for (auto it = GetNextInactiveTask();
       it != inactive_tasks_.end() && it->second->IsInside(position) &&
       (it->first.first < position || it->first.second <= max_priority);
       it = GetNextInactiveTask()) {
    SetTaskActive(it, true);
  }
}

void BarelyPerformer::RemoveTask(BarelyTask* task) noexcept {
  if (task->is_active) {
    [[maybe_unused]] const bool success =
        (active_tasks_.erase({{task->GetEndPosition(), task->priority}, task}) == 1);
    assert(success && "Failed to destroy active task");
    task->SetActive(false);
  } else {
    [[maybe_unused]] const bool success =
        (inactive_tasks_.erase({{task->position, task->priority}, task}) == 1);
    assert(success && "Failed to destroy inactive task");
  }
}

void BarelyPerformer::SetLoopBeginPosition(double new_loop_begin_position) noexcept {
  if (loop_begin_position == new_loop_begin_position) {
    return;
  }
  loop_begin_position = new_loop_begin_position;
  if (is_looping && position >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position));
  }
}

void BarelyPerformer::SetLoopLength(double new_loop_length) noexcept {
  new_loop_length = std::max(new_loop_length, 0.0);
  if (loop_length == new_loop_length) {
    return;
  }
  loop_length = new_loop_length;
  if (is_looping && position >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position));
  }
}

void BarelyPerformer::SetLooping(bool new_is_looping) noexcept {
  if (is_looping == new_is_looping) {
    return;
  }
  is_looping = new_is_looping;
  if (is_looping && position >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position));
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::SetPosition(double new_position) noexcept {
  if (position == new_position) {
    return;
  }
  if (is_looping && new_position >= GetLoopEndPosition()) {
    position = LoopAround(new_position);
    while (!active_tasks_.empty()) {
      SetTaskActive(active_tasks_.begin(), false);
    }
  } else {
    position = new_position;
    for (auto it = active_tasks_.begin(); it != active_tasks_.end();) {
      // Copy the values in case `it` gets invalidated after the `Process` call.
      auto [end_position, task] = *it;
      if (!task->IsInside(position)) {
        SetTaskActive(it, false);
      }
      it = active_tasks_.upper_bound({end_position, task});
    }
  }
}

void BarelyPerformer::SetTaskDuration(BarelyTask* task, double old_duration) noexcept {
  if (task->is_active) {
    const TaskKey old_task_key = {task->position + old_duration, task->priority};
    if (task->IsInside(position)) {
      UpdateActiveTaskKey(old_task_key, task);
    } else {
      SetTaskActive(active_tasks_.find({old_task_key, task}), false);
    }
  }
}

void BarelyPerformer::SetTaskPriority(BarelyTask* task, int old_priority) noexcept {
  const TaskKey old_task_key = {task->position, old_priority};
  if (task->is_active) {
    UpdateActiveTaskKey(old_task_key, task);
  } else {
    UpdateInactiveTaskKey(old_task_key, task);
  }
}

void BarelyPerformer::SetTaskPosition(BarelyTask* task, double old_position) noexcept {
  if (task->is_active) {
    const TaskKey old_task_key = {old_position + task->duration, task->priority};
    if (task->IsInside(position)) {
      UpdateActiveTaskKey(old_task_key, task);
    } else {
      SetTaskActive(active_tasks_.find({old_task_key, task}), false);
    }
  } else {
    UpdateInactiveTaskKey({old_position, task->priority}, task);
  }
}

void BarelyPerformer::Start() noexcept { is_playing = true; }

void BarelyPerformer::Stop() noexcept {
  is_playing = false;
  while (!active_tasks_.empty()) {
    SetTaskActive(active_tasks_.begin(), false);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::Update(double duration) noexcept {
  if (!is_playing) {
    return;
  }
  assert(duration > 0.0);
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert(!GetNextTaskKey() || duration <= GetNextTaskKey()->first);
  SetPosition(position + duration);
}

std::set<std::pair<BarelyPerformer::TaskKey, BarelyTask*>>::const_iterator
BarelyPerformer::GetNextInactiveTask() const noexcept {
  if (!is_playing) {
    return inactive_tasks_.end();
  }
  auto next_it =
      inactive_tasks_.lower_bound({{position, std::numeric_limits<int>::min()}, nullptr});
  // Check if any inactive task became active (in case a new position was set).
  // TODO(#147): This may be optimized further using an interval tree.
  for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
    if (it->second->GetEndPosition() > position) {
      return it;
    }
  }
  return next_it;
}

double BarelyPerformer::LoopAround(double new_position) const noexcept {
  return loop_length > 0.0
             ? loop_begin_position + std::fmod(new_position - loop_begin_position, loop_length)
             : loop_begin_position;
}

void BarelyPerformer::SetTaskActive(const std::set<std::pair<TaskKey, BarelyTask*>>::iterator& it,
                                    bool is_active) noexcept {
  BarelyTask* task = it->second;
  auto node = (is_active ? inactive_tasks_ : active_tasks_).extract(it);
  node.value().first.first = is_active ? task->GetEndPosition() : task->position;
  (is_active ? active_tasks_ : inactive_tasks_).insert(std::move(node));
  task->SetActive(is_active);
}

void BarelyPerformer::UpdateActiveTaskKey(TaskKey old_task_key, BarelyTask* task) noexcept {
  auto node = active_tasks_.extract({old_task_key, task});
  node.value().first = {task->GetEndPosition(), task->priority};
  active_tasks_.insert(std::move(node));
}

void BarelyPerformer::UpdateInactiveTaskKey(TaskKey old_task_key, BarelyTask* task) noexcept {
  auto node = inactive_tasks_.extract({old_task_key, task});
  node.value().first = {task->position, task->priority};
  inactive_tasks_.insert(std::move(node));
}
