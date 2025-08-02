#include "api/performer.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <optional>
#include <set>
#include <utility>

#include "api/engine.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyPerformer::BarelyPerformer(BarelyEngine& engine) noexcept : engine_(engine) {
  engine_.AddPerformer(this);
}

BarelyPerformer::~BarelyPerformer() noexcept { engine_.RemovePerformer(this); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::AddTask(BarelyTask* task) noexcept {
  [[maybe_unused]] const bool success =
      inactive_tasks_.emplace(TaskKey{task->GetPosition(), task->GetPriority()}, task).second;
  assert(success && "Failed to create task");
}

std::optional<BarelyPerformer::TaskKey> BarelyPerformer::GetNextTaskKey() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  const double loop_end_position = GetLoopEndPosition();
  std::optional<TaskKey> next_key = std::nullopt;

  // Check inactive tasks.
  if (!inactive_tasks_.empty()) {
    const auto next_it =
        inactive_tasks_.lower_bound({{position_, std::numeric_limits<int>::min()}, nullptr});
    // If the performer position is inside an inactive task, we can return immediately.
    // TODO(#147): This may be optimized further using an interval tree.
    for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
      if (it->second->GetEndPosition() > position_) {
        return TaskKey{0.0, it->first.second};
      }
    }
    // Loop around if needed.
    if (is_looping_ &&
        (next_it == inactive_tasks_.end() || next_it->first.first >= GetLoopEndPosition())) {
      if (const auto loop_it = inactive_tasks_.lower_bound(
              {{loop_begin_position_, std::numeric_limits<int>::min()}, nullptr});
          loop_it != inactive_tasks_.end() && loop_it->first.first < GetLoopEndPosition()) {
        next_key = {loop_it->first.first + loop_length_, loop_it->first.second};
      }
    } else if (next_it != inactive_tasks_.end()) {
      next_key = next_it->first;
    }
  }

  // Check active tasks.
  if (!active_tasks_.empty()) {
    if (const TaskKey next_active_key =
            is_looping_ ? TaskKey{std::min(active_tasks_.begin()->first.first, loop_end_position),
                                  active_tasks_.begin()->first.second}
                        : active_tasks_.begin()->first;
        !next_key.has_value() || next_active_key < next_key) {
      next_key = next_active_key;
    }
  }

  if (next_key.has_value()) {
    assert(next_key->first >= position_ && "Invalid next duration");
    return TaskKey{next_key->first - position_, next_key->second};
  }
  return std::nullopt;
}

void BarelyPerformer::ProcessAllTasksAtPosition(int max_priority) noexcept {
  if (!is_playing_) {
    return;
  }
  // Active tasks get processed in `SetPosition`, so we only need to process inactive tasks here.
  for (auto it = GetNextInactiveTask();
       it != inactive_tasks_.end() && it->second->IsInside(position_) &&
       (it->first.first < position_ || it->first.second <= max_priority);
       it = GetNextInactiveTask()) {
    SetTaskActive(it, true);
  }
}

void BarelyPerformer::RemoveTask(BarelyTask* task) noexcept {
  if (task->IsActive()) {
    [[maybe_unused]] const bool success =
        (active_tasks_.erase({{task->GetEndPosition(), task->GetPriority()}, task}) == 1);
    assert(success && "Failed to destroy active task");
    task->SetActive(false);
  } else {
    [[maybe_unused]] const bool success =
        (inactive_tasks_.erase({{task->GetPosition(), task->GetPriority()}, task}) == 1);
    assert(success && "Failed to destroy inactive task");
  }
}

void BarelyPerformer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) {
    return;
  }
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

void BarelyPerformer::SetLoopLength(double loop_length) noexcept {
  loop_length = std::max(loop_length, 0.0);
  if (loop_length_ == loop_length) {
    return;
  }
  loop_length_ = loop_length;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

void BarelyPerformer::SetLooping(bool is_looping) noexcept {
  if (is_looping_ == is_looping) {
    return;
  }
  is_looping_ = is_looping;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::SetPosition(double position) noexcept {
  if (position_ == position) {
    return;
  }
  if (is_looping_ && position >= GetLoopEndPosition()) {
    position_ = LoopAround(position);
    while (!active_tasks_.empty()) {
      SetTaskActive(active_tasks_.begin(), false);
    }
  } else {
    position_ = position;
    for (auto it = active_tasks_.begin(); it != active_tasks_.end();) {
      // Copy the values in case `it` gets invalidated after the `Process` call.
      auto [end_position, task] = *it;
      if (!task->IsInside(position_)) {
        SetTaskActive(it, false);
      }
      it = active_tasks_.upper_bound({end_position, task});
    }
  }
}

void BarelyPerformer::SetTaskDuration(BarelyTask* task, double old_duration) noexcept {
  if (task->IsActive()) {
    const TaskKey old_task_key = {task->GetPosition() + old_duration, task->GetPriority()};
    if (task->IsInside(position_)) {
      UpdateActiveTaskKey(old_task_key, task);
    } else {
      SetTaskActive(active_tasks_.find({old_task_key, task}), false);
    }
  }
}

void BarelyPerformer::SetTaskPriority(BarelyTask* task, int old_priority) noexcept {
  const TaskKey old_task_key = {task->GetPosition(), old_priority};
  if (task->IsActive()) {
    UpdateActiveTaskKey(old_task_key, task);
  } else {
    UpdateInactiveTaskKey(old_task_key, task);
  }
}

void BarelyPerformer::SetTaskPosition(BarelyTask* task, double old_position) noexcept {
  if (task->IsActive()) {
    const TaskKey old_task_key = {old_position + task->GetDuration(), task->GetPriority()};
    if (task->IsInside(position_)) {
      UpdateActiveTaskKey(old_task_key, task);
    } else {
      SetTaskActive(active_tasks_.find({old_task_key, task}), false);
    }
  } else {
    UpdateInactiveTaskKey({old_position, task->GetPriority()}, task);
  }
}

void BarelyPerformer::Start() noexcept { is_playing_ = true; }

void BarelyPerformer::Stop() noexcept {
  is_playing_ = false;
  while (!active_tasks_.empty()) {
    SetTaskActive(active_tasks_.begin(), false);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::Update(double duration) noexcept {
  if (!is_playing_) {
    return;
  }
  assert(duration > 0.0);
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert(!GetNextTaskKey() || duration <= GetNextTaskKey()->first);
  SetPosition(position_ + duration);
}

std::set<std::pair<BarelyPerformer::TaskKey, BarelyTask*>>::const_iterator
BarelyPerformer::GetNextInactiveTask() const noexcept {
  if (!is_playing_) {
    return inactive_tasks_.end();
  }
  auto next_it =
      inactive_tasks_.lower_bound({{position_, std::numeric_limits<int>::min()}, nullptr});
  // Check if any inactive task became active (in case a new position was set).
  // TODO(#147): This may be optimized further using an interval tree.
  for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
    if (it->second->GetEndPosition() > position_) {
      return it;
    }
  }
  return next_it;
}

double BarelyPerformer::LoopAround(double position) const noexcept {
  return loop_length_ > 0.0
             ? loop_begin_position_ + std::fmod(position - loop_begin_position_, loop_length_)
             : loop_begin_position_;
}

void BarelyPerformer::SetTaskActive(const std::set<std::pair<TaskKey, BarelyTask*>>::iterator& it,
                                    bool is_active) noexcept {
  BarelyTask* task = it->second;
  auto node = (is_active ? inactive_tasks_ : active_tasks_).extract(it);
  node.value().first.first = is_active ? task->GetEndPosition() : task->GetPosition();
  (is_active ? active_tasks_ : inactive_tasks_).insert(std::move(node));
  task->SetActive(is_active);
}

void BarelyPerformer::UpdateActiveTaskKey(TaskKey old_task_key, BarelyTask* task) noexcept {
  auto node = active_tasks_.extract({old_task_key, task});
  node.value().first = {task->GetEndPosition(), task->GetPriority()};
  active_tasks_.insert(std::move(node));
}

void BarelyPerformer::UpdateInactiveTaskKey(TaskKey old_task_key, BarelyTask* task) noexcept {
  auto node = inactive_tasks_.extract({old_task_key, task});
  node.value().first = {task->GetPosition(), task->GetPriority()};
  inactive_tasks_.insert(std::move(node));
}
