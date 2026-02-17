#ifndef BARELYMUSICIAN_ENGINE_TASK_STATE_H_
#define BARELYMUSICIAN_ENGINE_TASK_STATE_H_

#include <barelymusician.h>

#include <cstdint>

#include "core/callback.h"
#include "core/constants.h"

namespace barely {

struct TaskState {
  Callback<BarelyTaskEventCallback> callback = {};

  double position = 0.0;
  double duration = 0.0;
  int32_t priority = 0;

  uint32_t performer_index = kInvalidIndex;

  uint32_t prev_task_index = kInvalidIndex;
  uint32_t next_task_index = kInvalidIndex;

  // Denotes whether the task is active or not.
  bool is_active = false;

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

  bool IsInside(double other_position) const noexcept {
    return other_position >= position && other_position < GetEndPosition();
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TASK_STATE_H_
