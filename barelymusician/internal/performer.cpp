#include "barelymusician/internal/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <optional>
#include <set>
#include <tuple>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/task.h"

namespace barely::internal {

void Performer::CancelAllOneOffTasks() noexcept { one_off_tasks_.clear(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
Task* Performer::CreateTask(TaskDefinition definition, double position, int32_t process_order,
                            void* user_data) noexcept {
  auto task = std::make_unique<Task>(
      definition, position, process_order, user_data,
      [&](Task* task, double position) { SetTaskPosition(*task, position); },
      [&](Task* task, int process_order) { SetTaskProcessOrder(*task, process_order); });
  [[maybe_unused]] const auto [it, success] =
      recurring_tasks_.emplace(std::tuple{position, process_order, task.get()}, std::move(task));
  assert(success);
  return it->second.get();
}

bool Performer::DestroyTask(Task* task) noexcept {
  if (last_processed_recurring_task_it_ &&
      (*last_processed_recurring_task_it_)->second.get() == task) {
    const auto recurring_task_it = *last_processed_recurring_task_it_;
    PrevLastProcessedRecurringTaskIt();
    recurring_tasks_.erase(recurring_task_it);
    return true;
  } else {
    return recurring_tasks_.erase({task->GetPosition(), task->GetProcessOrder(), task}) == 1;
  }
}

std::optional<std::pair<double, int>> Performer::GetDurationToNextTask() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  // Check recurring tasks.
  std::optional<std::tuple<double, int, Task*>> next_task_key = std::nullopt;
  if (const auto next_recurring_task = GetNextRecurringTask();
      next_recurring_task != recurring_tasks_.end()) {
    next_task_key = next_recurring_task->first;
    if (is_looping_ &&
        (std::get<double>(*next_task_key) < position_ ||
         (last_processed_recurring_task_it_ &&
          next_recurring_task->first <= (*last_processed_recurring_task_it_)->first))) {
      // Loop around.
      if (loop_length_ > 0.0) {
        std::get<double>(*next_task_key) += loop_length_;
      } else {
        next_task_key = std::nullopt;
      }
    }
  }
  // Check one-off tasks.
  if (const auto next_one_off_task = one_off_tasks_.begin();
      next_one_off_task != one_off_tasks_.end() &&
      (!next_task_key || (next_one_off_task->first.first <= std::get<double>(*next_task_key) &&
                          next_one_off_task->first.second <= std::get<int>(*next_task_key)))) {
    std::get<double>(*next_task_key) = next_one_off_task->first.first;
    std::get<int>(*next_task_key) = next_one_off_task->first.second;
  }

  if (next_task_key) {
    return std::pair{std::get<double>(*next_task_key) - position_, std::get<int>(*next_task_key)};
  }
  return std::nullopt;
}

double Performer::GetLoopBeginPosition() const noexcept { return loop_begin_position_; }

double Performer::GetLoopLength() const noexcept { return loop_length_; }

double Performer::GetPosition() const noexcept { return position_; }

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

void Performer::ProcessNextTaskAtPosition() noexcept {
  if (!is_playing_) {
    return;
  }
  if (const auto it = one_off_tasks_.begin();
      it != one_off_tasks_.end() && it->first.first == position_) {
    // Process the next one-off task.
    it->second.Process();
    one_off_tasks_.erase(it);
    return;
  }
  if (const auto it = GetNextRecurringTask();
      it != recurring_tasks_.end() && it->second->GetPosition() == position_ &&
      (!last_processed_recurring_task_it_ || **last_processed_recurring_task_it_ < *it)) {
    // Process the next recurring task.
    it->second->Process();
    last_processed_recurring_task_it_ = it;
  }
}

void Performer::ScheduleOneOffTask(TaskDefinition definition, double position, int process_order,
                                   void* user_data) noexcept {
  if (position < position_) {
    return;
  }
  one_off_tasks_.emplace(std::pair{position, process_order}, OneOffTask(definition, user_data));
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
  one_off_tasks_.erase(one_off_tasks_.begin(),
                       one_off_tasks_.lower_bound({position, std::numeric_limits<int>::lowest()}));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_tasks_.empty()) {
      // Reset all remaining one-off tasks back to the beginning.
      for (auto it = one_off_tasks_.begin(); it != one_off_tasks_.end();) {
        auto current = it++;
        auto node = one_off_tasks_.extract(current);
        node.key().first = std::max(node.key().first - loop_length_, loop_begin_position_);
        one_off_tasks_.insert(std::move(node));
      }
    }
    position_ = LoopAround(position);
  } else {
    position_ = position;
  }
}

void Performer::SetTaskPosition(Task& task, double position) noexcept {
  if (last_processed_recurring_task_it_ &&
      &task == (*last_processed_recurring_task_it_)->second.get()) {
    PrevLastProcessedRecurringTaskIt();
  }
  auto node = recurring_tasks_.extract({task.GetPosition(), task.GetProcessOrder(), &task});
  std::get<double>(node.key()) = position;
  recurring_tasks_.insert(std::move(node));
}

void Performer::SetTaskProcessOrder(Task& task, int process_order) noexcept {
  if (last_processed_recurring_task_it_ &&
      &task == (*last_processed_recurring_task_it_)->second.get()) {
    PrevLastProcessedRecurringTaskIt();
  }
  auto node = recurring_tasks_.extract({task.GetPosition(), task.GetProcessOrder(), &task});
  std::get<int>(node.key()) = process_order;
  recurring_tasks_.insert(std::move(node));
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
  assert(duration >= 0.0 &&
         // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
         (!GetDurationToNextTask().has_value() || duration <= GetDurationToNextTask()->first));
  if (const double next_position = position_ + duration; next_position > position_) {
    SetPosition(next_position);
  }
}

Performer::RecurringTaskMap::const_iterator Performer::GetNextRecurringTask() const noexcept {
  auto next_it =
      last_processed_recurring_task_it_
          ? std::next(*last_processed_recurring_task_it_)
          : recurring_tasks_.lower_bound({position_, std::numeric_limits<int>::lowest(), nullptr});
  if (is_looping_ && (next_it == recurring_tasks_.end() ||
                      next_it->second->GetPosition() >= loop_begin_position_ + loop_length_)) {
    // Loop back to the beginning.
    next_it = recurring_tasks_.lower_bound(
        {loop_begin_position_, std::numeric_limits<int>::lowest(), nullptr});
  }
  return next_it;
}

double Performer::LoopAround(double position) const noexcept {
  return loop_length_ > 0.0
             ? loop_begin_position_ + std::fmod(position - loop_begin_position_, loop_length_)
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
