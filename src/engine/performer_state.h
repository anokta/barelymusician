#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_

#include <barelymusician.h>

#include <cfloat>
#include <cmath>
#include <cstdint>

#include "core/callback.h"
#include "core/constants.h"

namespace barely {

inline constexpr double kMinTaskDuration = DBL_EPSILON;

struct PerformerState {
 public:
  double loop_begin_position = 0.0;
  double loop_length = 1.0;
  double position = 0.0;

  uint32_t first_active_task_index = kInvalidIndex;
  uint32_t first_inactive_task_index = kInvalidIndex;

  bool is_looping = false;
  bool is_playing = false;

  [[nodiscard]] double LoopAround(double new_position) const noexcept {
    return loop_length > 0.0
               ? loop_begin_position + std::fmod(new_position - loop_begin_position, loop_length)
               : loop_begin_position;
  }

  [[nodiscard]] double GetLoopEndPosition() const noexcept {
    return loop_begin_position + loop_length;
  }
};

struct TaskState {
  Callback<BarelyTaskCallback> callback = {};

  double position = 0.0;
  double duration = kMinTaskDuration;
  int32_t priority = 0;

  uint32_t performer_index = kInvalidIndex;

  uint32_t prev_task_index = kInvalidIndex;
  uint32_t next_task_index = kInvalidIndex;

  // Denotes whether the task is active or not.
  bool is_active = false;

  [[nodiscard]] double GetEndPosition() const noexcept { return position + duration; }

  [[nodiscard]] bool IsActiveBefore(const TaskState& other) const noexcept {
    const double end_position = GetEndPosition();
    const double other_end_position = other.GetEndPosition();
    return end_position < other_end_position ||
           (end_position == other_end_position && priority < other.priority);
  }

  [[nodiscard]] bool IsInactiveBefore(const TaskState& other) const noexcept {
    return position < other.position || (position == other.position && priority < other.priority);
  }

  [[nodiscard]] bool IsInside(double other_position) const noexcept {
    return other_position >= position && other_position < GetEndPosition();
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
