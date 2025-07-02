#include "api/performer.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
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
  [[maybe_unused]] const bool success = inactive_tasks_.emplace(task->GetPosition(), task).second;
  assert(success && "Failed to create task");
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::AddTrigger(BarelyTrigger* trigger) noexcept {
  [[maybe_unused]] const bool success = triggers_.emplace(trigger->GetPosition(), trigger).second;
  assert(success && "Failed to create trigger");
}

std::optional<double> BarelyPerformer::GetNextDuration() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  const double loop_end_position = GetLoopEndPosition();
  std::optional<double> next_position = std::nullopt;

  // Check inactive tasks.
  if (!inactive_tasks_.empty()) {
    const auto next_it = inactive_tasks_.lower_bound({position_, nullptr});
    // If the performer position is inside an inactive task, we can return immediately.
    // TODO(#147): This may be optimized further using an interval tree.
    for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
      if (it->second->GetEndPosition() > position_) {
        return 0.0f;
      }
    }
    // Loop around if needed.
    if (is_looping_ &&
        (next_it == inactive_tasks_.end() || next_it->first >= GetLoopEndPosition())) {
      if (const auto loop_it = inactive_tasks_.lower_bound({loop_begin_position_, nullptr});
          loop_it != inactive_tasks_.end() && loop_it->first < GetLoopEndPosition()) {
        next_position = loop_it->first + loop_length_;
      }
    } else if (next_it != inactive_tasks_.end()) {
      next_position = next_it->first;
    }
  }

  // Check active tasks.
  if (!active_tasks_.empty()) {
    if (const double next_active_task_position =
            is_looping_ ? std::min(active_tasks_.begin()->first, loop_end_position)
                        : active_tasks_.begin()->first;
        !next_position.has_value() || next_active_task_position < *next_position) {
      next_position = next_active_task_position;
    }
  }

  // Check triggers.
  if (!triggers_.empty()) {
    const auto next_it = (last_trigger_it_ != triggers_.end())
                             ? std::next(last_trigger_it_)
                             : triggers_.lower_bound({position_, nullptr});
    // Loop around if needed.
    if (is_looping_ && (next_it == triggers_.end() || next_it->first >= GetLoopEndPosition())) {
      if (const auto loop_it = triggers_.lower_bound({loop_begin_position_, nullptr});
          loop_it != triggers_.end() && loop_it->first < GetLoopEndPosition() &&
          (!next_position.has_value() || next_it->first < *next_position)) {
        next_position = loop_it->first + loop_length_;
      }
    } else if (next_it != triggers_.end() &&
               (!next_position.has_value() || next_it->first < *next_position)) {
      next_position = next_it->first;
    }
  }

  if (next_position.has_value()) {
    assert(*next_position >= position_ && "Invalid next duration");
    return *next_position - position_;
  }
  return std::nullopt;
}

void BarelyPerformer::ProcessAllTasksAtPosition() noexcept {
  if (!is_playing_) {
    return;
  }
  // Process all the triggers first.
  bool has_processed_triggers = false;
  for (auto it = GetNextTrigger(); it != triggers_.end() && it->first < GetLoopEndPosition();
       it = GetNextTrigger()) {
    has_processed_triggers = true;
    last_trigger_it_ = it;
    it->second->Process();
  }
  if (has_processed_triggers) {
    return;
  }
  // Active tasks get processed in `SetPosition`, so we only need to process inactive tasks here.
  for (auto it = GetNextInactiveTask();
       it != inactive_tasks_.end() && it->second->IsInside(position_); it = GetNextInactiveTask()) {
    SetTaskActive(it, true);
  }
}

void BarelyPerformer::RemoveTask(BarelyTask* task) noexcept {
  if (task->IsActive()) {
    [[maybe_unused]] const bool success =
        (active_tasks_.erase({task->GetEndPosition(), task}) == 1);
    assert(success && "Failed to destroy active task");
    task->SetActive(false);
  } else {
    [[maybe_unused]] const bool success = (inactive_tasks_.erase({task->GetPosition(), task}) == 1);
    assert(success && "Failed to destroy inactive task");
  }
}

void BarelyPerformer::RemoveTrigger(BarelyTrigger* trigger) noexcept {
  if (last_trigger_it_ != triggers_.end() && last_trigger_it_->second == trigger) {
    // Update the last processed iterator to the previous trigger.
    last_trigger_it_ =
        (last_trigger_it_ == triggers_.begin()) ? triggers_.end() : std::prev(last_trigger_it_);
  }
  [[maybe_unused]] const bool success = (triggers_.erase({trigger->GetPosition(), trigger}) == 1);
  assert(success && "Failed to destroy trigger");
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
  last_trigger_it_ = triggers_.end();
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
        task->Process(BarelyTaskState_kUpdate);
      }
      it = active_tasks_.upper_bound({end_position, task});
    }
  }
}

void BarelyPerformer::SetTaskDuration(BarelyTask* task, double old_duration) noexcept {
  if (task->IsActive()) {
    const double old_end_position = task->GetPosition() + old_duration;
    if (task->IsInside(position_)) {
      UpdateActiveTaskKey(old_end_position, task);
    } else {
      SetTaskActive(active_tasks_.find({old_end_position, task}), false);
    }
  }
}

void BarelyPerformer::SetTaskPosition(BarelyTask* task, double old_position) noexcept {
  if (task->IsActive()) {
    const double old_end_position = old_position + task->GetDuration();
    if (task->IsInside(position_)) {
      UpdateActiveTaskKey(old_end_position, task);
    } else {
      SetTaskActive(active_tasks_.find({old_end_position, task}), false);
    }
  } else {
    UpdateInactiveTaskKey(old_position, task);
  }
}

void BarelyPerformer::SetTriggerPosition(BarelyTrigger* trigger, double old_position) noexcept {
  if (last_trigger_it_ != triggers_.end() && last_trigger_it_->second == trigger) {
    // Update the last processed iterator to the previous trigger.
    last_trigger_it_ =
        (last_trigger_it_ == triggers_.begin()) ? triggers_.end() : std::prev(last_trigger_it_);
  }
  auto node = triggers_.extract({old_position, trigger});
  node.value().first = trigger->GetPosition();
  triggers_.insert(std::move(node));
}

void BarelyPerformer::Start() noexcept { is_playing_ = true; }

void BarelyPerformer::Stop() noexcept {
  is_playing_ = false;
  last_trigger_it_ = triggers_.end();
  while (!active_tasks_.empty()) {
    SetTaskActive(active_tasks_.begin(), false);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyPerformer::Update(double duration) noexcept {
  if (!is_playing_) {
    return;
  }
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert(duration > 0.0 && (!GetNextDuration() || duration <= GetNextDuration()));
  SetPosition(position_ + duration);
}

std::set<std::pair<double, BarelyTask*>>::const_iterator BarelyPerformer::GetNextInactiveTask()
    const noexcept {
  if (!is_playing_) {
    return inactive_tasks_.end();
  }
  auto next_it = inactive_tasks_.lower_bound({position_, nullptr});
  // Check if any inactive task became active (in case a new position was set).
  // TODO(#147): This may be optimized further using an interval tree.
  for (auto it = inactive_tasks_.begin(); it != next_it; ++it) {
    if (it->second->GetEndPosition() > position_) {
      return it;
    }
  }
  return next_it;
}

std::set<std::pair<double, BarelyTrigger*>>::const_iterator BarelyPerformer::GetNextTrigger()
    const noexcept {
  if (!is_playing_) {
    return triggers_.end();
  }
  return (last_trigger_it_ != triggers_.end()) ? std::next(last_trigger_it_)
                                               : triggers_.lower_bound({position_, nullptr});
}

double BarelyPerformer::LoopAround(double position) const noexcept {
  return loop_length_ > 0.0
             ? loop_begin_position_ + std::fmod(position - loop_begin_position_, loop_length_)
             : loop_begin_position_;
}

void BarelyPerformer::SetTaskActive(const std::set<std::pair<double, BarelyTask*>>::iterator& it,
                                    bool is_active) noexcept {
  BarelyTask* task = it->second;
  assert(!is_playing_ || ((is_active && task->IsInside(position_)) ||
                          (!is_active && (!task->IsInside(position_) ||
                                          task->GetEndPosition() >= GetLoopEndPosition()))));
  auto node = (is_active ? inactive_tasks_ : active_tasks_).extract(it);
  node.value().first = is_active ? task->GetEndPosition() : task->GetPosition();
  (is_active ? active_tasks_ : inactive_tasks_).insert(std::move(node));
  task->SetActive(is_active);
}

void BarelyPerformer::UpdateActiveTaskKey(double old_end_position, BarelyTask* task) noexcept {
  auto node = active_tasks_.extract({old_end_position, task});
  node.value().first = task->GetEndPosition();
  active_tasks_.insert(std::move(node));
}

void BarelyPerformer::UpdateInactiveTaskKey(double old_position, BarelyTask* task) noexcept {
  auto node = inactive_tasks_.extract({old_position, task});
  node.value().first = task->GetPosition();
  inactive_tasks_.insert(std::move(node));
}
