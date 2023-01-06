#include "barelymusician/engine/performer.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/common/find_or_null.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Performer::AddTask(Id id, double position,
                        TaskCallback callback) noexcept {
  assert(id > kInvalid);
  if (positions_.emplace(id, position).second) {
    callbacks_.emplace(std::pair{position, id}, std::move(callback));
    return true;
  }
  return false;
}

double Performer::GetDurationToNextTask() const noexcept {
  double next_position = std::numeric_limits<double>::max();
  if (!is_playing_) {
    return next_position;
  }

  // Check next tasks.
  if (const auto next_callback = GetNextTaskCallback();
      next_callback != callbacks_.end()) {
    next_position = next_callback->first.first;
    if (is_looping_ && (next_position < position_ ||
                        (last_triggered_position_ &&
                         *last_triggered_position_ == next_position))) {
      next_position += loop_length_;
    }
  }
  // Check one-off tasks.
  if (!one_off_callbacks_.empty() &&
      one_off_callbacks_.begin()->first < next_position) {
    next_position = one_off_callbacks_.begin()->first;
  }

  if (next_position < std::numeric_limits<double>::max()) {
    return next_position - position_;
  }
  return next_position;
}

const Performer::TaskCallback* Performer::GetTaskCallback(
    Id id) const noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    return FindOrNull(callbacks_, std::pair{*position, id});
  }
  return nullptr;
}

const double* Performer::GetTaskPosition(Id id) const noexcept {
  return FindOrNull(positions_, id);
}

double Performer::GetLoopBeginPosition() const noexcept {
  return loop_begin_position_;
}

double Performer::GetLoopLength() const noexcept { return loop_length_; }

double Performer::GetPosition() const noexcept { return position_; }

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

bool Performer::RemoveTask(Id id) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    callbacks_.erase(std::pair{position_it->second, id});
    positions_.erase(position_it);
    return true;
  }
  return false;
}

bool Performer::ScheduleOneOffTask(double position,
                                   TaskCallback callback) noexcept {
  if (position >= position_ && callback) {
    one_off_callbacks_.emplace(position, std::move(callback));
    return true;
  }
  return false;
}

bool Performer::SetTaskCallback(Id id, TaskCallback callback) noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    *FindOrNull(callbacks_, std::pair{*position, id}) = std::move(callback);
    return true;
  }
  return false;
}

bool Performer::SetTaskPosition(Id id, double position) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    if (position_it->second != position) {
      const auto it = callbacks_.find(std::pair{position_it->second, id});
      auto node = callbacks_.extract(it);
      node.key().first = position;
      callbacks_.insert(std::move(node));
      position_it->second = position;
    }
    return true;
  }
  return false;
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
  assert(loop_length > 0.0);
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
  last_triggered_position_ = std::nullopt;
  one_off_callbacks_.erase(one_off_callbacks_.begin(),
                           one_off_callbacks_.lower_bound(position));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_callbacks_.empty()) {
      // Reset all remaining one-off callbacks back to `loop_begin_position_`.
      std::multimap<double, TaskCallback> remaining_callbacks;
      for (auto& it : one_off_callbacks_) {
        remaining_callbacks.emplace(loop_begin_position_, std::move(it.second));
      }
      one_off_callbacks_.swap(remaining_callbacks);
    }
    position_ = loop_begin_position_ +
                std::fmod(position - loop_begin_position_, loop_length_);
  } else {
    position_ = position;
  }
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept { is_playing_ = false; }

void Performer::TriggerAllTasksAtCurrentPosition() noexcept {
  // Trigger one-off tasks.
  if (!one_off_callbacks_.empty()) {
    auto it = one_off_callbacks_.begin();
    while (it != one_off_callbacks_.end() && it->first <= position_) {
      it->second();
      ++it;
    }
    one_off_callbacks_.erase(one_off_callbacks_.begin(), it);
  }
  // Trigger next tasks.
  auto callback = GetNextTaskCallback();
  while (callback != callbacks_.end() && callback->first.first <= position_) {
    if (callback->second) {
      callback->second();
    }
    last_triggered_position_ = callback->first.first;
    ++callback;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::Update(double duration) noexcept {
  if (is_playing_) {
    assert(duration >= 0.0 && duration <= GetDurationToNextTask());
    SetPosition(position_ + duration);
  }
}

std::map<std::pair<double, Id>, Performer::TaskCallback>::const_iterator
Performer::GetNextTaskCallback() const noexcept {
  auto it = callbacks_.lower_bound(std::pair{position_, kInvalid});
  if (last_triggered_position_) {
    while (it != callbacks_.end() &&
           it->first.first == *last_triggered_position_) {
      ++it;
    }
  }
  if (it == callbacks_.end() && is_looping_) {
    // Loop back to `loop_begin_position_`.
    it = callbacks_.lower_bound(std::pair{loop_begin_position_, kInvalid});
  }
  return it;
}

}  // namespace barely::internal
