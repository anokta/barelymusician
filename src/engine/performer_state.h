#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_

#include <barelymusician.h>

#include <cmath>
#include <cstdint>

#include "core/callback.h"
#include "engine/task_state.h"

namespace barely {

struct PerformerState {
 public:
  // Loop begin position in beats.
  double loop_begin_position = 0.0;

  // Loop length in beats.
  double loop_length = 1.0;

  // Position in beats.
  double position = 0.0;

  // First active task index.
  uint32_t first_active_task_index = UINT32_MAX;

  // First inactive task index.
  uint32_t first_inactive_task_index = UINT32_MAX;

  // Denotes whether performer is looping or not.
  bool is_looping = false;

  // Denotes whether performer is playing or not.
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

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
