#include "barelymusician/internal/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/id.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::CreateTask(Id task_id, TaskDefinition definition,
                           bool is_one_off, double position, int process_order,
                           void* user_data) noexcept {
  assert(task_id > kInvalid);
  assert(!is_one_off || position >= position_);
  [[maybe_unused]] auto success =
      infos_.emplace(task_id, TaskInfo{is_one_off, position, process_order})
          .second;
  assert(success);
  success = (is_one_off ? one_off_tasks_ : recurring_tasks_)
                .emplace(TaskKey{position, process_order, task_id},
                         Task(definition, user_data))
                .second;
  assert(success);
}

bool Performer::DestroyTask(Id task_id) noexcept {
  if (task_id == kInvalid) {
    return false;
  }
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    if (last_processed_recurring_task_it_ &&
        (*last_processed_recurring_task_it_)->first.task_id == task_id) {
      const auto recurring_task_it = *last_processed_recurring_task_it_;
      PrevLastProcessedRecurringTaskIt();
      recurring_tasks_.erase(recurring_task_it);
    } else {
      const auto& [is_one_off, position, process_order] = it->second;
      [[maybe_unused]] const auto success =
          (is_one_off ? one_off_tasks_ : recurring_tasks_)
              .erase({position, process_order, task_id}) == 1;
      assert(success);
    }
    infos_.erase(it);
    return true;
  }
  return false;
}

std::optional<std::pair<double, int>> Performer::GetDurationToNextTask()
    const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  std::optional<TaskMap::key_type> next_task_key = std::nullopt;

  // Check recurring tasks.
  if (const auto next_recurring_task = GetNextRecurringTask();
      next_recurring_task != recurring_tasks_.end()) {
    next_task_key = next_recurring_task->first;
    if (is_looping_ && (next_task_key->position < position_ ||
                        (last_processed_recurring_task_it_ &&
                         next_recurring_task->first <=
                             (*last_processed_recurring_task_it_)->first))) {
      // Loop around.
      if (loop_length_ > 0.0) {
        next_task_key->position += loop_length_;
      } else {
        next_task_key = std::nullopt;
      }
    }
  }
  // Check one-off tasks.
  if (const auto next_one_off_task = one_off_tasks_.begin();
      next_one_off_task != one_off_tasks_.end() &&
      (!next_task_key || next_one_off_task->first <= *next_task_key)) {
    next_task_key = next_one_off_task->first;
  }

  if (next_task_key) {
    return std::pair{next_task_key->position - position_,
                     next_task_key->process_order};
  }
  return std::nullopt;
}

double Performer::GetLoopBeginPosition() const noexcept {
  return loop_begin_position_;
}

double Performer::GetLoopLength() const noexcept { return loop_length_; }

double Performer::GetPosition() const noexcept { return position_; }

std::optional<double> Performer::GetTaskPosition(Id task_id) const noexcept {
  if (task_id == kInvalid) {
    return std::nullopt;
  }
  if (const auto* info = FindOrNull(infos_, task_id)) {
    return info->position;
  }
  return std::nullopt;
}

std::optional<int> Performer::GetTaskProcessOrder(Id task_id) const noexcept {
  if (task_id == kInvalid) {
    return std::nullopt;
  }
  if (const auto* info = FindOrNull(infos_, task_id)) {
    return info->process_order;
  }
  return std::nullopt;
}

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

void Performer::ProcessNextTaskAtPosition() noexcept {
  if (!is_playing_) {
    return;
  }
  if (const auto it = one_off_tasks_.begin();
      it != one_off_tasks_.end() && it->first.position == position_) {
    // Process the next one-off task.
    [[maybe_unused]] const auto success = infos_.erase(it->first.task_id) == 1;
    assert(success);
    it->second.Process();
    one_off_tasks_.erase(it);
    return;
  }
  if (const auto it = GetNextRecurringTask();
      it != recurring_tasks_.end() && it->first.position == position_ &&
      (!last_processed_recurring_task_it_ ||
       (*last_processed_recurring_task_it_)->first < it->first)) {
    // Process the next recurring task.
    const_cast<Task&>(it->second).Process();
    last_processed_recurring_task_it_ = it;
  }
}

void Performer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) {
    return;
  }
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ > loop_begin_position_) {
    if (loop_length_ > 0.0 && position_ > loop_begin_position_ + loop_length_) {
      last_processed_recurring_task_it_ = std::nullopt;
    }
    position_ = LoopAround(position_);
  }
}

void Performer::SetLoopLength(double loop_length) noexcept {
  loop_length = std::max(loop_length, 0.0);
  if (loop_length_ == loop_length) {
    return;
  }
  loop_length_ = loop_length;
  if (is_looping_ && position_ > loop_begin_position_) {
    if (loop_length_ > 0.0 && position_ > loop_begin_position_ + loop_length_) {
      last_processed_recurring_task_it_ = std::nullopt;
    }
    position_ = LoopAround(position_);
  }
}

void Performer::SetLooping(bool is_looping) noexcept {
  if (is_looping_ == is_looping) {
    return;
  }
  is_looping_ = is_looping;
  if (is_looping_ && position_ > loop_begin_position_) {
    if (loop_length_ > 0.0 && position_ > loop_begin_position_ + loop_length_) {
      last_processed_recurring_task_it_ = std::nullopt;
    }
    position_ = LoopAround(position_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::SetPosition(double position) noexcept {
  last_processed_recurring_task_it_ = std::nullopt;
  if (position_ == position) {
    return;
  }
  one_off_tasks_.erase(
      one_off_tasks_.begin(),
      one_off_tasks_.lower_bound(
          {position, std::numeric_limits<int>::lowest(), kInvalid}));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_tasks_.empty()) {
      // Reset all remaining one-off tasks back to the beginning.
      for (auto it = one_off_tasks_.begin(); it != one_off_tasks_.end();) {
        auto current = it++;
        auto node = one_off_tasks_.extract(current);
        node.key().position =
            std::max(node.key().position - loop_length_, loop_begin_position_);
        one_off_tasks_.insert(std::move(node));
      }
    }
    position_ = LoopAround(position);
  } else {
    position_ = position;
  }
}

bool Performer::SetTaskPosition(Id task_id, double position) noexcept {
  if (task_id == kInvalid) {
    return false;
  }
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    auto& [is_one_off, current_position, process_order] = it->second;
    if (is_one_off && position < position_) {
      // Position is in the past.
      return false;
    }
    if (current_position != position) {
      if (last_processed_recurring_task_it_ &&
          task_id == (*last_processed_recurring_task_it_)->first.task_id) {
        PrevLastProcessedRecurringTaskIt();
      }
      auto& tasks = (is_one_off ? one_off_tasks_ : recurring_tasks_);
      auto node = tasks.extract({current_position, process_order, task_id});
      node.key().position = position;
      tasks.insert(std::move(node));
      current_position = position;
    }
    return true;
  }
  return false;
}

bool Performer::SetTaskProcessOrder(Id task_id, int process_order) noexcept {
  if (task_id == kInvalid) {
    return false;
  }
  if (const auto it = infos_.find(task_id); it != infos_.end()) {
    auto& [is_one_off, position, current_process_order] = it->second;
    if (current_process_order != process_order) {
      if (last_processed_recurring_task_it_ &&
          task_id == (*last_processed_recurring_task_it_)->first.task_id) {
        PrevLastProcessedRecurringTaskIt();
      }
      auto& tasks = (is_one_off ? one_off_tasks_ : recurring_tasks_);
      auto node = tasks.extract({position, current_process_order, task_id});
      node.key().process_order = process_order;
      tasks.insert(std::move(node));
      current_process_order = process_order;
    }
    return true;
  }
  return false;
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept {
  is_playing_ = false;
  last_processed_recurring_task_it_ = std::nullopt;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (!is_playing_) {
    return;
  }
  assert(duration >= 0.0 && (!GetDurationToNextTask() ||
                             duration <= GetDurationToNextTask()->first));
  if (const double next_position = position_ + duration;
      next_position > position_) {
    SetPosition(next_position);
  }
}

Performer::TaskMap::const_iterator Performer::GetNextRecurringTask()
    const noexcept {
  auto next_it =
      last_processed_recurring_task_it_
          ? std::next(*last_processed_recurring_task_it_)
          : recurring_tasks_.lower_bound(
                {position_, std::numeric_limits<int>::lowest(), kInvalid});
  if (is_looping_ &&
      (next_it == recurring_tasks_.end() ||
       next_it->first.position >= loop_begin_position_ + loop_length_)) {
    // Loop back to the beginning.
    next_it = recurring_tasks_.lower_bound(
        {loop_begin_position_, std::numeric_limits<int>::lowest(), kInvalid});
  }
  return next_it;
}

double Performer::LoopAround(double position) const noexcept {
  return loop_length_ > 0.0
             ? loop_begin_position_ +
                   std::fmod(position - loop_begin_position_, loop_length_)
             : loop_begin_position_;
}

void Performer::PrevLastProcessedRecurringTaskIt() noexcept {
  assert(last_processed_recurring_task_it_.has_value());
  if (*last_processed_recurring_task_it_ != recurring_tasks_.begin()) {
    --(*last_processed_recurring_task_it_);
  } else {
    last_processed_recurring_task_it_ = std::nullopt;
  }
}

}  // namespace barely::internal
