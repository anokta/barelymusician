#include "engine/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>

#include "barelymusician.h"

namespace barely::internal {

void Performer::Task::SetDuration(double duration) noexcept {
  assert(duration > 0.0 && "Invalid task duration");
  if (duration != duration_) {
    const double old_duration = duration_;
    duration_ = duration;
    performer_.SetTaskDuration(this, old_duration);
  }
}

void Performer::Task::SetPosition(double position) noexcept {
  if (position != position_) {
    const double old_position = position_;
    position_ = position;
    performer_.SetTaskPosition(this, old_position);
  }
}

void Performer::Task::SetProcessCallback(ProcessCallback callback) noexcept {
  if (is_active_) {
    Process(TaskState::kEnd);
  }
  process_callback_ = callback;
  if (is_active_) {
    Process(TaskState::kBegin);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Performer::Task* Performer::CreateTask(double position, double duration,
                                       Task::ProcessCallback callback) noexcept {
  auto task = std::make_unique<Task>(*this, position, duration, callback);
  Task* task_ptr = task.get();
  [[maybe_unused]] const bool success = tasks_.emplace(task_ptr, std::move(task)).second;
  assert(success && "Failed to create task");
  inactive_tasks_.emplace(position, task_ptr);
  return task_ptr;
}

void Performer::DestroyTask(Task* task) noexcept {
  if (task->IsActive()) {
    [[maybe_unused]] const bool success =
        (active_tasks_.erase({task->GetEndPosition(), task}) == 1);
    assert(success && "Failed to destroy active task");
    task->SetActive(false);
  } else {
    [[maybe_unused]] const bool success = (inactive_tasks_.erase({task->GetPosition(), task}) == 1);
    assert(success && "Failed to destroy inactive task");
  }
  tasks_.erase(task);
}

std::optional<double> Performer::GetNextDuration() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  std::optional<double> next_position = std::nullopt;

  // Check inactive tasks.
  if (const auto next_it = GetNextInactiveTask(); next_it != inactive_tasks_.end()) {
    if (next_it->second->IsInside(position_)) {
      // Performer position is inside an inactive task, we can return immediately.
      return 0.0;
    }
    if (is_looping_ && next_it->first < position_) {
      if (loop_length_ > 0.0) {
        // Loop around.
        next_position = next_it->first + loop_length_;
      }
    } else {
      next_position = next_it->first;
    }
  }

  // Check active tasks.
  if (!active_tasks_.empty()) {
    if (const double next_active_task_position =
            is_looping_ ? std::min(active_tasks_.begin()->first, GetLoopEndPosition())
                        : active_tasks_.begin()->first;
        !next_position.has_value() || next_active_task_position < *next_position) {
      next_position = next_active_task_position;
    }
  }

  // Check beat callback.
  if (beat_callback_) {
    std::optional<double> next_beat_position =
        (last_beat_position_ == position_) ? std::ceil(position_ + 1.0) : std::ceil(position_);
    if (is_looping_ && *next_beat_position >= GetLoopEndPosition()) {
      const double first_beat_offset = std::ceil(loop_begin_position_) - loop_begin_position_;
      next_beat_position = (loop_length_ > first_beat_offset)
                               ? std::optional<double>{first_beat_offset + GetLoopEndPosition()}
                               : std::nullopt;
    }
    if (next_beat_position.has_value() &&
        (!next_position.has_value() || *next_beat_position < *next_position)) {
      next_position = next_beat_position;
    }
  }

  if (next_position) {
    assert(*next_position >= position_ && "Invalid next duration");
    return *next_position - position_;
  }
  return std::nullopt;
}

void Performer::ProcessAllTasksAtPosition() noexcept {
  if (!is_playing_) {
    return;
  }
  if (last_beat_position_ != position_ && std::ceil(position_) == position_) {
    last_beat_position_ = position_;
    beat_callback_();
  }
  // Active tasks get processed in `SetPosition`, so we only need to process inactive tasks here.
  for (auto it = GetNextInactiveTask();
       it != inactive_tasks_.end() && it->second->IsInside(position_); it = GetNextInactiveTask()) {
    SetTaskActive(it, true);
  }
}

void Performer::SetBeatCallback(BeatCallback callback) noexcept { beat_callback_ = callback; }

void Performer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) {
    return;
  }
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

void Performer::SetLoopLength(double loop_length) noexcept {
  loop_length = std::max(loop_length, 0.0);
  if (loop_length_ == loop_length) {
    return;
  }
  loop_length_ = loop_length;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

void Performer::SetLooping(bool is_looping) noexcept {
  if (is_looping_ == is_looping) {
    return;
  }
  is_looping_ = is_looping;
  if (is_looping_ && position_ >= GetLoopEndPosition()) {
    SetPosition(LoopAround(position_));
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::SetPosition(double position) noexcept {
  last_beat_position_ = std::nullopt;
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
      } else {
        task->Process(TaskState::kUpdate);
      }
      it = active_tasks_.upper_bound({end_position, task});
    }
  }
}

void Performer::SetTaskActive(std::set<std::pair<double, Task*>>::iterator it,
                              bool is_active) noexcept {
  Task* task = it->second;
  assert(!is_playing_ ||
         ((is_active && task->IsInside(position_)) || (!is_active && !task->IsInside(position_))));
  auto node = (is_active ? inactive_tasks_ : active_tasks_).extract(it);
  node.value().first = is_active ? task->GetEndPosition() : task->GetPosition();
  (is_active ? active_tasks_ : inactive_tasks_).insert(std::move(node));
  task->SetActive(is_active);
}

void Performer::SetTaskDuration(Task* task, double old_duration) noexcept {
  if (task->IsActive() && !task->IsInside(position_)) {
    SetTaskActive(active_tasks_.find({task->GetPosition() + old_duration, task}), false);
  }
}

void Performer::SetTaskPosition(Task* task, double old_position) noexcept {
  if (task->IsActive() && !task->IsInside(position_)) {
    SetTaskActive(active_tasks_.find({old_position + task->GetDuration(), task}), false);
  }
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept {
  is_playing_ = false;
  last_beat_position_ = std::nullopt;
  while (!active_tasks_.empty()) {
    SetTaskActive(active_tasks_.begin(), false);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (!is_playing_) {
    return;
  }
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert(duration > 0.0 && (!GetNextDuration() || duration <= GetNextDuration()));
  SetPosition(position_ + duration);
}

std::set<std::pair<double, Performer::Task*>>::const_iterator Performer::GetNextInactiveTask()
    const noexcept {
  auto next_it = inactive_tasks_.lower_bound({position_, nullptr});
  // Check if any inactive task became active (in case a new position was set).
  // TODO(#147): This may be optimized further using an interval tree.
  for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
    if (it->second->GetEndPosition() > position_) {
      return it;
    }
  }
  // Loop back to the beginning if needed.
  if (is_looping_ && (next_it == inactive_tasks_.end() || next_it->first >= GetLoopEndPosition())) {
    next_it = inactive_tasks_.lower_bound({loop_begin_position_, nullptr});
  }
  return next_it;
}

double Performer::LoopAround(double position) const noexcept {
  return loop_length_ > 0.0
             ? loop_begin_position_ + std::fmod(position - loop_begin_position_, loop_length_)
             : loop_begin_position_;
}

}  // namespace barely::internal
