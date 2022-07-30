#include "barelymusician/engine/sequencer.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/common/find_or_null.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Sequencer::AddEvent(Id id, double position,
                         EventCallback callback) noexcept {
  assert(id > kInvalid);
  assert(callback);
  if (positions_.emplace(id, position).second) {
    callbacks_.emplace(std::pair{position, id}, std::move(callback));
    return true;
  }
  return false;
}

double Sequencer::GetDurationToNextEvent() const noexcept {
  double next_position = std::numeric_limits<double>::max();
  if (!is_playing_) {
    return next_position;
  }

  // Check next events.
  if (const auto next_callback = GetNextEventCallback();
      next_callback != callbacks_.end()) {
    next_position = next_callback->first.first;
    if (is_looping_ && (next_position < position_ ||
                        (last_triggered_position_ &&
                         *last_triggered_position_ == position_))) {
      next_position += loop_length_;
    }
  }
  // Check one-off events.
  if (!one_off_callbacks_.empty() &&
      one_off_callbacks_.begin()->first < next_position) {
    next_position = one_off_callbacks_.begin()->first;
  }

  if (next_position < std::numeric_limits<double>::max()) {
    return next_position - position_;
  }
  return next_position;
}

const Sequencer::EventCallback* Sequencer::GetEventCallback(
    Id id) const noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    return FindOrNull(callbacks_, std::pair{*position, id});
  }
  return nullptr;
}

const double* Sequencer::GetEventPosition(Id id) const noexcept {
  return FindOrNull(positions_, id);
}

double Sequencer::GetLoopBeginPosition() const noexcept {
  return loop_begin_position_;
}

double Sequencer::GetLoopLength() const noexcept { return loop_length_; }

double Sequencer::GetPosition() const noexcept { return position_; }

bool Sequencer::IsLooping() const noexcept { return is_looping_; }

bool Sequencer::IsPlaying() const noexcept { return is_playing_; }

bool Sequencer::RemoveEvent(Id id) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    callbacks_.erase(std::pair{position_it->second, id});
    positions_.erase(position_it);
    return true;
  }
  return false;
}

bool Sequencer::ScheduleOneOffEvent(double position,
                                    EventCallback callback) noexcept {
  assert(callback);
  if (is_playing_ && position >= position_) {
    one_off_callbacks_.emplace(position, std::move(callback));
    return true;
  }
  return false;
}

bool Sequencer::SetEventCallback(Id id, EventCallback callback) noexcept {
  assert(callback);
  if (const auto* position = FindOrNull(positions_, id)) {
    *FindOrNull(callbacks_, std::pair{*position, id}) = std::move(callback);
    return true;
  }
  return false;
}

bool Sequencer::SetEventPosition(Id id, double position) noexcept {
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

void Sequencer::SetLoopBeginPosition(double loop_begin_position) noexcept {
  if (loop_begin_position_ == loop_begin_position) return;
  loop_begin_position_ = loop_begin_position;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

void Sequencer::SetLoopLength(double loop_length) noexcept {
  assert(loop_length > 0.0);
  if (loop_length_ == loop_length) return;
  loop_length_ = loop_length;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

void Sequencer::SetLooping(bool is_looping) noexcept {
  if (is_looping_ == is_looping) return;
  is_looping_ = is_looping;
  if (is_looping_ && position_ > loop_begin_position_) {
    position_ = loop_begin_position_ +
                std::fmod(position_ - loop_begin_position_, loop_length_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Sequencer::SetPosition(double position) noexcept {
  if (position_ == position) return;
  last_triggered_position_ = std::nullopt;
  one_off_callbacks_.erase(one_off_callbacks_.begin(),
                           one_off_callbacks_.find(position));
  if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
    if (!one_off_callbacks_.empty()) {
      // Reset all remaining one-off callbacks back to `loop_begin_position_`.
      std::multimap<double, EventCallback> remaining_callbacks;
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

void Sequencer::Start() noexcept { is_playing_ = true; }

void Sequencer::Stop() noexcept {
  one_off_callbacks_.clear();
  is_playing_ = false;
}

void Sequencer::TriggerAllEventsAtCurrentPosition() noexcept {
  // Trigger one-off events.
  if (!one_off_callbacks_.empty()) {
    auto it = one_off_callbacks_.begin();
    while (it != one_off_callbacks_.end() && it->first <= position_) {
      it->second(position_);
      ++it;
    }
    one_off_callbacks_.erase(one_off_callbacks_.begin(), it);
  }
  // Trigger next events.
  auto callback = GetNextEventCallback();
  while (callback != callbacks_.end() && callback->first.first <= position_) {
    callback->second(position_);
    ++callback;
  }
  last_triggered_position_ = position_;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Sequencer::Update(double duration) noexcept {
  if (is_playing_) {
    assert(duration >= 0.0 && duration <= GetDurationToNextEvent());
    SetPosition(position_ + duration);
  }
}

std::map<std::pair<double, Id>, Sequencer::EventCallback>::const_iterator
Sequencer::GetNextEventCallback() const noexcept {
  auto it = callbacks_.lower_bound(std::pair{position_, kInvalid});
  if (it == callbacks_.end() && is_looping_) {
    // Loop back to `loop_begin_position_`.
    it = callbacks_.lower_bound(std::pair{loop_begin_position_, kInvalid});
  }
  return it;
}

}  // namespace barely::internal
