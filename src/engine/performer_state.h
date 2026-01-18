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
  double loop_begin_position = 0.0;
  double loop_length = 1.0;
  double position = 0.0;

  uint32_t first_active_task_index = UINT32_MAX;
  uint32_t first_inactive_task_index = UINT32_MAX;

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

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
