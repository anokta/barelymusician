#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
void Performer::CreateTask(Id task_id, TaskDefinition definition,
                           double position, bool is_one_off,
                           void* user_data) noexcept {
  assert(task_id > kInvalid);
  auto success =
      position_type_pairs_.emplace(task_id, std::pair{position, is_one_off})
          .second;
  assert(success);
  success = (is_one_off ? one_off_tasks_ : tasks_)
                .emplace(std::piecewise_construct,
                         std::forward_as_tuple(position, task_id),
                         std::forward_as_tuple(definition, user_data))
                .second;
  assert(success);
}

// double Performer::GetDurationToNextTask() const noexcept {
//   double next_position = std::numeric_limits<double>::max();
//   if (!is_playing_) {
//     return next_position;
//   }

//   // Check next tasks.
//   if (const auto next_callback = GetNextTaskCallback();
//       next_callback != callbacks_.end()) {
//     next_position = next_callback->first.first;
//     if (is_looping_ && (next_position < position_ ||
//                         (last_triggered_position_ &&
//                          *last_triggered_position_ == next_position))) {
//       next_position += loop_length_;
//     }
//   }
//   // Check one-off tasks.
//   if (!one_off_callbacks_.empty() &&
//       one_off_callbacks_.begin()->first < next_position) {
//     next_position = one_off_callbacks_.begin()->first;
//   }

//   if (next_position < std::numeric_limits<double>::max()) {
//     return next_position - position_;
//   }
//   return next_position;
// }

Status Performer::DestroyTask(Id task_id) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = position_type_pairs_.find(task_id);
      it != position_type_pairs_.end()) {
    const auto success = (it->second.second ? one_off_tasks_ : tasks_)
                             .erase(std::pair{it->second.first, task_id}) == 1;
    assert(success);
    position_type_pairs_.erase(it);
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
  if (const auto* position_type_pair =
          FindOrNull(position_type_pairs_, task_id)) {
    return position_type_pair->first;
  }
  return {Status::kNotFound};
}

bool Performer::IsLooping() const noexcept { return is_looping_; }

bool Performer::IsPlaying() const noexcept { return is_playing_; }

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
  // TODO(#109): Refactor to match `Task` functionality.
  // last_triggered_position_ = std::nullopt;
  // one_off_callbacks_.erase(one_off_callbacks_.begin(),
  //                          one_off_callbacks_.lower_bound(position));
  // if (is_looping_ && position >= loop_begin_position_ + loop_length_) {
  //   if (!one_off_callbacks_.empty()) {
  //     // Reset all remaining one-off callbacks back to
  //     `loop_begin_position_`. std::multimap<double, TaskCallback>
  //     remaining_callbacks; for (auto& it : one_off_callbacks_) {
  //       remaining_callbacks.emplace(loop_begin_position_,
  //       std::move(it.second));
  //     }
  //     one_off_callbacks_.swap(remaining_callbacks);
  //   }
  //   position_ = loop_begin_position_ +
  //               std::fmod(position - loop_begin_position_, loop_length_);
  // } else {
  //   position_ = position;
  // }
  position_ = position;
}

Status Performer::SetTaskPosition(Id task_id, double position) noexcept {
  if (task_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = position_type_pairs_.find(task_id);
      it != position_type_pairs_.end()) {
    auto& [current_position, is_one_off] = it->second;
    if (current_position != position) {
      auto& tasks = (is_one_off ? one_off_tasks_ : tasks_);
      auto node = tasks.extract(std::pair{current_position, task_id});
      node.key().first = position;
      tasks.insert(std::move(node));
      current_position = position;
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Performer::Start() noexcept { is_playing_ = true; }

void Performer::Stop() noexcept { is_playing_ = false; }

// void Performer::TriggerAllTasksAtCurrentPosition() noexcept {
//   // Trigger one-off tasks.
//   if (!one_off_callbacks_.empty()) {
//     auto it = one_off_callbacks_.begin();
//     while (it != one_off_callbacks_.end() && it->first <= position_) {
//       it->second();
//       ++it;
//     }
//     one_off_callbacks_.erase(one_off_callbacks_.begin(), it);
//   }
//   // Trigger next tasks.
//   auto callback = GetNextTaskCallback();
//   while (callback != callbacks_.end() && callback->first.first <= position_)
//   {
//     if (callback->second) {
//       callback->second();
//     }
//     last_triggered_position_ = callback->first.first;
//     ++callback;
//   }
// }

// // NOLINTNEXTLINE(bugprone-exception-escape)
// void Performer::Update(double duration) noexcept {
//   if (is_playing_) {
//     assert(duration >= 0.0 && duration <= GetDurationToNextTask());
//     SetPosition(position_ + duration);
//   }
// }

// std::map<std::pair<double, Id>, Performer::TaskCallback>::const_iterator
// Performer::GetNextTaskCallback() const noexcept {
//   auto it = callbacks_.lower_bound(std::pair{position_, kInvalid});
//   if (last_triggered_position_) {
//     while (it != callbacks_.end() &&
//            it->first.first == *last_triggered_position_) {
//       ++it;
//     }
//   }
//   if (it == callbacks_.end() && is_looping_) {
//     // Loop back to `loop_begin_position_`.
//     it = callbacks_.lower_bound(std::pair{loop_begin_position_, kInvalid});
//   }
//   return it;
// }

}  // namespace barely::internal
