#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_

#include <barelymusician.h>

#include <cmath>
#include <cstdint>
#include <set>
#include <utility>

#include "core/callback.h"
#include "engine/task_state.h"

namespace barely {

/// Task key consists of a position and its priority.
using TaskKey = std::pair<double, int32_t>;

struct PerformerState {
 public:
  // Set of task position-index pairs.
  std::set<std::pair<TaskKey, uint32_t>> active_tasks;
  std::set<std::pair<TaskKey, uint32_t>> inactive_tasks;

  // Loop begin position in beats.
  double loop_begin_position = 0.0;

  // Loop length in beats.
  double loop_length = 1.0;

  // Position in beats.
  double position = 0.0;

  // Denotes whether performer is looping or not.
  bool is_looping = false;

  // Denotes whether performer is playing or not.
  bool is_playing = false;

  // Loops around a given `new_position`.
  [[nodiscard]] double LoopAround(double new_position) const noexcept {
    return loop_length > 0.0
               ? loop_begin_position + std::fmod(new_position - loop_begin_position, loop_length)
               : loop_begin_position;
  }

  /// Returns loop end position.
  ///
  /// @return Loop end position in beats.
  [[nodiscard]] double GetLoopEndPosition() const noexcept {
    return loop_begin_position + loop_length;
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
