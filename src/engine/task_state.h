#ifndef BARELYMUSICIAN_ENGINE_TASK_STATE_H_
#define BARELYMUSICIAN_ENGINE_TASK_STATE_H_

#include <barelymusician.h>

#include <cstdint>

#include "core/callback.h"

namespace barely {

struct TaskState {
  /// Task event callback.
  Callback<BarelyTaskEventCallback> callback = {};

  /// Task position in beats.
  double position = 0.0;

  /// Task duration in beats.
  double duration = 0.0;

  /// Task priority.
  int32_t priority = 0;

  /// Performer index.
  uint32_t performer_index = UINT32_MAX;

  /// Previous task index.
  uint32_t prev_task_index = UINT32_MAX;

  /// Next task index.
  uint32_t next_task_index = UINT32_MAX;

  /// Denotes whether the task is active or not.
  bool is_active = false;

  /// Returns the end position.
  ///
  /// @return End position in beats.
  double GetEndPosition() const noexcept { return position + duration; }

  bool IsActiveBefore(const TaskState& other) const noexcept {
    const double end_position = GetEndPosition();
    const double other_end_position = other.GetEndPosition();
    return end_position < other_end_position ||
           (end_position == other_end_position && priority < other.priority);
  }

  bool IsInactiveBefore(const TaskState& other) const noexcept {
    return position < other.position || (position == other.position && priority < other.priority);
  }

  /// Returns whether a position is inside the task boundaries.
  ///
  /// @param position Position in beats.
  /// @return True if inside, false otherwise.
  bool IsInside(double other_position) const noexcept {
    return other_position >= position && other_position < GetEndPosition();
  }

  /// Sets the event callback.
  ///
  /// @param new_callback New callback.
  /// @param new_user_data New user data.
  void SetEventCallback(BarelyTaskEventCallback new_callback, void* new_user_data) noexcept {
    if (is_active) {
      callback(BarelyTaskEventType_kEnd);
    }
    callback = {new_callback, new_user_data};
    if (is_active) {
      callback(BarelyTaskEventType_kBegin);
    }
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TASK_STATE_H_
