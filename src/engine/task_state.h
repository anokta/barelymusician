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
  uint32_t performer_index = 0;

  /// Denotes whether the task is active or not.
  bool is_active = false;

  /// Returns the end position.
  ///
  /// @return End position in beats.
  double GetEndPosition() const noexcept { return position + duration; }

  /// Returns whether a position is inside the task boundaries.
  ///
  /// @param position Position in beats.
  /// @return True if inside, false otherwise.
  bool IsInside(double other_position) const noexcept {
    return other_position >= position && other_position < GetEndPosition();
  }

  /// Sets whether the task is currently active or not.
  ///
  /// @param new_is_active True if active, false otherwise.
  void SetActive(bool new_is_active) noexcept {
    is_active = new_is_active;
    callback(is_active ? BarelyTaskEventType_kBegin : BarelyTaskEventType_kEnd);
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

// TODO(#126): temp shortcut
using TaskState = ::barely::TaskState;

#endif  // BARELYMUSICIAN_ENGINE_TASK_STATE_H_
