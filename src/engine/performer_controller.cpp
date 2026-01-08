#include "engine/performer_controller.h"

#include <barelymusician.h>

#include <cstdint>

namespace barely {

BarelyRef PerformerController::Acquire() noexcept {
  const uint32_t performer_index = performer_pool_.Acquire();
  if (performer_index == 0) {
    return {};
  }

  PerformerState& performer = performer_pool_.Get(performer_index);
  performer = {};

  return {performer_index, performer_pool_.GetGeneration(performer_index)};
}

void PerformerController::Release(uint32_t performer_index) noexcept {
  // TODO(#126): remove all tasks
  performer_pool_.Release(performer_index);
}

BarelyRef PerformerController::AcquireTask(uint32_t performer_index, double position,
                                           double duration, int32_t priority,
                                           BarelyTaskEventCallback callback,
                                           void* user_data) noexcept {
  const uint32_t task_index = task_pool_.Acquire();
  if (task_index == 0) {
    return {};
  }

  TaskState& task = task_pool_.Get(task_index);
  task = {{callback, user_data}, position, duration, priority, performer_index};
  performer_pool_.Get(performer_index).AddTask(&task);

  return {task_index, task_pool_.GetGeneration(task_index)};
}

void PerformerController::ReleaseTask(uint32_t task_index) noexcept {
  TaskState& task = task_pool_.Get(task_index);
  performer_pool_.Get(task.performer_index).RemoveTask(&task);
  task_pool_.Release(task_index);
}

}  // namespace barely
