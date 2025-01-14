#include "engine/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>

#include "barelymusician.h"
#include "engine/event.h"

namespace barely::internal {

Performer::Task::Task(Performer& performer, const TaskEvent& task_event, double position) noexcept
    : Event<TaskEvent>(task_event), performer_(performer), position_(position) {}

void Performer::Task::SetPosition(double position) noexcept {
  if (position != position_) {
    performer_.SetTaskPosition(this, position);
    position_ = position;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Performer::Task* Performer::CreateTask(const TaskEvent& task_event, double position) noexcept {
  auto task = std::make_unique<Task>(*this, task_event, position);
  auto* task_ptr = task.get();
  [[maybe_unused]] const bool success =
      recurring_tasks_.emplace(std::pair{position, task_ptr}, std::move(task)).second;
  assert(success);
  return task_ptr;
}

void Performer::DestroyTask(Task* task) noexcept {
  if (last_processed_recurring_task_it_ &&
      (*last_processed_recurring_task_it_)->second.get() == task) {
    const auto recurring_task_it = *last_processed_recurring_task_it_;
    PrevLastProcessedRecurringTaskIt();
    recurring_tasks_.erase(recurring_task_it);
  } else {
    recurring_tasks_.erase({task->GetPosition(), task});
  }
}

std::optional<double> Performer::GetDurationToNextTask() const noexcept {
  if (!is_playing_) {
    return std::nullopt;
  }

  // Check recurring tasks.
  std::optional<double> next_task_position = std::nullopt;
  if (const auto next_recurring_task = GetNextRecurringTask();
      next_recurring_task != recurring_tasks_.end()) {
    next_task_position = next_recurring_task->first.first;
    if (is_looping_ && (*next_task_position < position_ ||
                        (last_processed_recurring_task_it_ &&
                         *next_recurring_task <= **last_processed_recurring_task_it_))) {
      // Loop around.
      if (loop_length_ > 0.0) {
        *next_task_position += loop_length_;
      } else {
        next_task_position = std::nullopt;
      }
    }
  }

  // Check beat callback.
  // TODO(#147): POC-only, this can be cleaned up after task refactor.
  if (beat_callback_) {
    double next_beat_position =
        (last_beat_position_ == position_) ? std::ceil(position_ + 1.0) : std::ceil(position_);
    if (is_looping_ && next_beat_position > loop_begin_position_ + loop_length_) {
      const double first_beat_offset = std::ceil(loop_begin_position_) - loop_begin_position_;
      next_beat_position = (loop_length_ > first_beat_offset)
                               ? first_beat_offset + loop_begin_position_ + loop_length_
                               : std::numeric_limits<double>::max();
    }
    if (next_beat_position < std::numeric_limits<double>::max() &&
        (!next_task_position.has_value() || next_beat_position < *next_task_position)) {
      next_task_position = next_beat_position;
    }
  }

  if (next_task_position) {
    return *next_task_position - position_;
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
  // TODO(#147): POC-only, this can be cleaned up after task refactor.
  if (last_beat_position_ != position_ && std::ceil(position_) == position_) {
    last_beat_position_ = position_;
    beat_callback_();
  }
  if (const auto it = GetNextRecurringTask();
      it != recurring_tasks_.end() && it->second->GetPosition() == position_ &&
      (!last_processed_recurring_task_it_ || **last_processed_recurring_task_it_ < *it)) {
    // Process the next recurring task.
    it->second->Process();
    last_processed_recurring_task_it_ = it;
  }
}

void Performer::SetBeatCallback(BeatCallback beat_callback) noexcept {
  beat_callback_ = beat_callback;
}

void Performer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) {
    return;
  }
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ > loop_begin_position_) {
    if (loop_length_ > 0.0 && position_ > loop_begin_position_ + loop_length_) {
      last_processed_recurring_task_it_ = std::nullopt;
      last_beat_position_ = std::nullopt;
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
      last_beat_position_ = std::nullopt;
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
      last_beat_position_ = std::nullopt;
      last_processed_recurring_task_it_ = std::nullopt;
    }
    position_ = LoopAround(position_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::SetPosition(double position) noexcept {
  last_beat_position_ = std::nullopt;
  last_processed_recurring_task_it_ = std::nullopt;
  if (position_ == position) {
    return;
  }
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    position_ = LoopAround(position);
  } else {
    position_ = position;
  }
}

void Performer::SetTaskPosition(Task* task, double position) noexcept {
  if (last_processed_recurring_task_it_ &&
      task == (*last_processed_recurring_task_it_)->second.get()) {
    PrevLastProcessedRecurringTaskIt();
  }
  auto node = recurring_tasks_.extract({task->GetPosition(), task});
  node.key().first = position;
  recurring_tasks_.insert(std::move(node));
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept {
  is_playing_ = false;
  last_beat_position_ = std::nullopt;
  last_processed_recurring_task_it_ = std::nullopt;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (!is_playing_) {
    return;
  }
  assert(duration >= 0.0 &&
         // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
         (!GetDurationToNextTask() || duration <= GetDurationToNextTask()));
  if (const double next_position = position_ + duration; next_position > position_) {
    SetPosition(next_position);
  }
}

Performer::RecurringTaskMap::const_iterator Performer::GetNextRecurringTask() const noexcept {
  auto next_it = last_processed_recurring_task_it_
                     ? std::next(*last_processed_recurring_task_it_)
                     : recurring_tasks_.lower_bound({position_, nullptr});
  if (is_looping_ && (next_it == recurring_tasks_.end() ||
                      next_it->first.first >= loop_begin_position_ + loop_length_)) {
    // Loop back to the beginning.
    next_it = recurring_tasks_.lower_bound({loop_begin_position_, nullptr});
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
