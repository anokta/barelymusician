#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::CreateTask(Id task_id, TaskDefinition definition,
                           double position, int process_order, void* user_data,
                           bool is_one_off) noexcept {
  assert(task_id > kInvalid);
  assert(!is_one_off || position >= position_);
  auto success =
      infos_.emplace(task_id, TaskInfo{is_one_off, position, process_order})
          .second;
  assert(success);
  success = (is_one_off ? one_off_tasks_ : recurring_tasks_)
                .emplace(std::tuple{position, process_order, task_id},
                         std::make_unique<Task>(definition, user_data))
                .second;
  assert(success);
}

std::optional<std::pair<double, int>> Performer::GetDurationToNextTask()
    const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  std::optional<std::tuple<double, int, Id>> next_task_key = std::nullopt;

  // Check recurring tasks.
  if (const auto next_recurring_task = GetNextRecurringTask();
      next_recurring_task != recurring_tasks_.end()) {
    next_task_key = next_recurring_task->first;
    auto& next_task_position = std::get<double>(*next_task_key);
    if (is_looping_ && (next_task_position < position_ ||
                        next_task_position == last_processed_position_)) {
      next_task_position += loop_length_;
    }
  }
  // Check one-off tasks.
  if (!one_off_tasks_.empty() &&
      (!next_task_key || one_off_tasks_.begin()->first <= *next_task_key)) {
    next_task_key = one_off_tasks_.begin()->first;
  }

  if (next_task_key) {
    return std::pair{std::get<double>(*next_task_key) - position_,
                     std::get<int>(*next_task_key)};
  }
  return std::nullopt;
}

Status Performer::DestroyTask(Id task_id) noexcept {
  if (task_id == kInvalid) return Status::InvalidArgument();
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    const auto success =
        (it->second.is_one_off ? one_off_tasks_ : recurring_tasks_)
            .erase(std::tuple{it->second.position, it->second.process_order,
                              task_id}) == 1;
    assert(success);
    infos_.erase(it);
    return Status::Ok();
  }
  return Status::NotFound();
}

double Performer::GetLoopBeginPosition() const noexcept {
  return loop_begin_position_;
}

double Performer::GetLoopLength() const noexcept { return loop_length_; }

double Performer::GetPosition() const noexcept { return position_; }

StatusOr<double> Performer::GetTaskPosition(Id task_id) const noexcept {
  if (task_id == kInvalid) return Status::InvalidArgument();
  if (const auto* info = FindOrNull(infos_, task_id)) {
    return info->position;
  }
  return Status::NotFound();
}

StatusOr<int> Performer::GetTaskProcessOrder(Id task_id) const noexcept {
  if (task_id == kInvalid) return Status::InvalidArgument();
  if (const auto* info = FindOrNull(infos_, task_id)) {
    return info->process_order;
  }
  return Status::NotFound();
}

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

void Performer::ProcessNextTaskAtPosition() noexcept {
  if (!is_playing_) {
    return;
  }
  if (const auto it = one_off_tasks_.begin();
      it != one_off_tasks_.end() && std::get<double>(it->first) == position_) {
    // Process the next one-off task.
    const auto success = infos_.erase(std::get<Id>(it->first)) == 1;
    assert(success);
    it->second->Process();
    one_off_tasks_.erase(it);
    return;
  }
  if (const auto it = GetNextRecurringTask();
      it != recurring_tasks_.end() &&
      std::get<double>(it->first) == position_) {
    // Process the next recurring task.
    it->second->Process();
    last_processed_position_ = std::get<double>(it->first);
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
      one_off_tasks_.lower_bound(std::tuple{
          is_looping_ ? std::min(position, loop_begin_position_ + loop_length_)
                      : position,
          std::numeric_limits<int>::lowest(), kInvalid}));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_tasks_.empty()) {
      // Reset all remaining one-off tasks back to the beginning.
      TaskMap remaining_one_off_tasks;
      for (auto& it : one_off_tasks_) {
        remaining_one_off_tasks.emplace(
            std::tuple{loop_begin_position_, std::numeric_limits<int>::lowest(),
                       std::get<Id>(it.first)},
            std::move(it.second));
        infos_.at(std::get<Id>(it.first)).position = loop_begin_position_;
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
  if (task_id == kInvalid) return Status::InvalidArgument();
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    auto& [is_one_off, current_position, order] = it->second;
    if (is_one_off && position < position_) {
      // Position is in the past.
      return Status::InvalidArgument();
    }
    if (current_position != position) {
      auto& tasks = (is_one_off ? one_off_tasks_ : recurring_tasks_);
      auto node = tasks.extract(std::tuple{current_position, order, task_id});
      std::get<double>(node.key()) = position;
      tasks.insert(std::move(node));
      current_position = position;
    }
    return Status::Ok();
  }
  return Status::NotFound();
}

Status Performer::SetTaskProcessOrder(Id task_id, int process_order) noexcept {
  if (task_id == kInvalid) return Status::InvalidArgument();
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    auto& [is_one_off, position, current_process_order] = it->second;
    if (current_process_order != process_order) {
      auto& tasks = (is_one_off ? one_off_tasks_ : recurring_tasks_);
      auto node =
          tasks.extract(std::tuple{position, current_process_order, task_id});
      std::get<int>(node.key()) = process_order;
      tasks.insert(std::move(node));
      current_process_order = process_order;
    }
    return Status::Ok();
  }
  return Status::NotFound();
}

void Performer::Start() noexcept {
  last_processed_position_ = std::nullopt;
  is_playing_ = true;
}

void Performer::Stop() noexcept { is_playing_ = false; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (is_playing_) {
    assert(duration >= 0.0 && (!GetDurationToNextTask() ||
                               duration <= GetDurationToNextTask()->first));
    SetPosition(position_ + duration);
  }
}

Performer::TaskMap::const_iterator Performer::GetNextRecurringTask()
    const noexcept {
  auto it = recurring_tasks_.lower_bound(
      std::tuple{position_, std::numeric_limits<int>::lowest(), kInvalid});
  if (last_processed_position_) {
    // Skip processed tasks.
    while (it != recurring_tasks_.end() &&
           std::get<double>(it->first) == *last_processed_position_) {
      ++it;
    }
  }
  if (it == recurring_tasks_.end() && is_looping_) {
    // Loop back to the beginning.
    it = recurring_tasks_.lower_bound(std::tuple{
        loop_begin_position_, std::numeric_limits<int>::lowest(), kInvalid});
  }
  return it;
}

}  // namespace barely::internal
