#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::CreateTask(Id task_id, TaskDefinition definition,
                           double position, TaskType type,
                           void* user_data) noexcept {
  assert(task_id > kInvalid);
  auto success = infos_.emplace(task_id, TaskInfo{position, type}).second;
  assert(success);
  success = (type == TaskType::kOneOff ? one_off_tasks_ : recurring_tasks_)
                .emplace(std::pair{position, task_id},
                         std::make_unique<Task>(definition, user_data))
                .second;
  assert(success);
}

std::optional<double> Performer::GetDurationToNextTask() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  std::optional<double> next_task_position = std::nullopt;

  // Check recurring tasks.
  if (const auto next_recurring_task = GetNextRecurringTask();
      next_recurring_task != recurring_tasks_.end()) {
    next_task_position = next_recurring_task->first.first;
    if (is_looping_ && (*next_task_position < position_ ||
                        next_task_position == last_processed_position_)) {
      *next_task_position += loop_length_;
    }
  }
  // Check one-off tasks.
  if (!one_off_tasks_.empty() &&
      (!next_task_position ||
       one_off_tasks_.begin()->first.first < *next_task_position)) {
    next_task_position = one_off_tasks_.begin()->first.first;
  }

  if (next_task_position) {
    return *next_task_position - position_;
  }
  return std::nullopt;
}

Status Performer::DestroyTask(Id task_id) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    const auto success =
        (it->second.type == TaskType::kOneOff ? one_off_tasks_
                                              : recurring_tasks_)
            .erase(std::pair{it->second.position, task_id}) == 1;
    assert(success);
    infos_.erase(it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

double Performer::GetLoopBeginPosition() const noexcept {
  return loop_begin_position_;
}

double Performer::GetLoopLength() const noexcept { return loop_length_; }

double Performer::GetPosition() const noexcept { return position_; }

StatusOr<double> Performer::GetTaskPosition(Id task_id) const noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto* info = FindOrNull(infos_, task_id)) {
    return info->position;
  }
  return {Status::kNotFound};
}

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

void Performer::ProcessAllTasksAtCurrentPosition() noexcept {
  // Process one-off tasks.
  if (!one_off_tasks_.empty()) {
    auto it = one_off_tasks_.begin();
    while (it != one_off_tasks_.end() && it->first.first <= position_) {
      it->second->Process();
      ++it;
    }
    one_off_tasks_.erase(one_off_tasks_.begin(), it);
  }
  // Process recurring tasks.
  auto it = GetNextRecurringTask();
  while (it != recurring_tasks_.end() && it->first.first <= position_) {
    it->second->Process();
    last_processed_position_ = it->first.first;
    ++it;
  }
}

void Performer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) return;
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

void Performer::SetLoopLength(double loop_length) noexcept {
  loop_length = std::max(loop_length, 0.0);
  if (loop_length_ == loop_length) return;
  loop_length_ = loop_length;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

void Performer::SetLooping(bool is_looping) noexcept {
  if (is_looping_ == is_looping) return;
  is_looping_ = is_looping;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::SetPosition(double position) noexcept {
  if (position_ == position) return;
  last_processed_position_ = std::nullopt;
  one_off_tasks_.erase(
      one_off_tasks_.begin(),
      one_off_tasks_.lower_bound(std::pair{position, kInvalid}));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_tasks_.empty()) {
      // Reset all remaining one-off tasks back to the beginning.
      TaskMap remaining_one_off_tasks;
      for (auto& it : one_off_tasks_) {
        remaining_one_off_tasks.emplace(
            std::pair{loop_begin_position_, it.first.second},
            std::move(it.second));
      }
      one_off_tasks_.swap(remaining_one_off_tasks);
    }
    position_ = loop_begin_position_ +
                std::fmod(position - loop_begin_position_, loop_length_);
  } else {
    position_ = position;
  }
}

Status Performer::SetTaskPosition(Id task_id, double position) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    auto& [current_position, type] = it->second;
    if (current_position != position) {
      auto& tasks =
          (type == TaskType::kOneOff ? one_off_tasks_ : recurring_tasks_);
      auto node = tasks.extract(std::pair{current_position, task_id});
      node.key().first = position;
      tasks.insert(std::move(node));
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept { is_playing_ = false; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (is_playing_) {
    assert(duration >= 0.0 && duration <= GetDurationToNextTask());
    SetPosition(position_ + duration);
  }
}

TaskMap::const_iterator Performer::GetNextRecurringTask() const noexcept {
  auto it = recurring_tasks_.lower_bound(std::pair{position_, kInvalid});
  if (last_processed_position_) {
    // Skip processed tasks.
    while (it != recurring_tasks_.end() &&
           it->first.first == *last_processed_position_) {
      ++it;
    }
  }
  if (it == recurring_tasks_.end() && is_looping_) {
    // Loop back to the beginning.
    it =
        recurring_tasks_.lower_bound(std::pair{loop_begin_position_, kInvalid});
  }
  return it;
}

}  // namespace barely::internal
