#include "engine/performer_controller.h"

#include <barelymusician.h>

#include <cstdint>

#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

uint32_t PerformerController::Acquire() noexcept {
  const uint32_t performer_index = engine_.performer_pool.Acquire();
  if (performer_index == 0) {
    return 0;
  }

  PerformerState& performer = engine_.performer_pool.Get(performer_index);
  performer = {};

  return performer_index;
}

void PerformerController::Release(uint32_t performer_index) noexcept {
  // TODO(#126): remove all tasks
  engine_.performer_pool.Release(performer_index);
}

uint32_t PerformerController::AcquireTask(uint32_t performer_index, double position,
                                          double duration, int32_t priority,
                                          BarelyTaskEventCallback callback,
                                          void* user_data) noexcept {
  const uint32_t task_index = engine_.task_pool.Acquire();
  if (task_index == 0) {
    return 0;
  }

  TaskState& task = engine_.task_pool.Get(task_index);
  task = {{callback, user_data}, position, duration, priority, performer_index};
  engine_.performer_pool.Get(performer_index).AddTask(&task);

  return task_index;
}

void PerformerController::ReleaseTask(uint32_t task_index) noexcept {
  TaskState& task = engine_.task_pool.Get(task_index);
  engine_.performer_pool.Get(task.performer_index).RemoveTask(&task);
  engine_.task_pool.Release(task_index);
}

}  // namespace barely
