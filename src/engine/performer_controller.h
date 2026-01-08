#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>
#include <limits>
#include <optional>

#include "core/pool.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

class PerformerController {
 public:
  [[nodiscard]] BarelyRef Acquire() noexcept;
  void Release(uint32_t performer_index) noexcept;

  [[nodiscard]] BarelyRef AcquireTask(uint32_t performer_index, double position, double duration,
                                      int32_t priority, BarelyTaskEventCallback callback,
                                      void* user_data) noexcept;
  void ReleaseTask(uint32_t task_index) noexcept;

  void SetTaskDuration(uint32_t task_index, double duration) noexcept {
    auto& task = task_pool_.Get(task_index);
    performer_pool_.Get(task.performer_index).SetTaskDuration(&task, duration);
  }
  void SetTaskEventCallback(uint32_t task_index, BarelyTaskEventCallback callback,
                            void* user_data) noexcept {
    task_pool_.Get(task_index).SetEventCallback(callback, user_data);
  }
  void SetTaskPosition(uint32_t task_index, double position) noexcept {
    auto& task = task_pool_.Get(task_index);
    performer_pool_.Get(task.performer_index).SetTaskPosition(&task, position);
  }
  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept {
    auto& task = task_pool_.Get(task_index);
    performer_pool_.Get(task.performer_index).SetTaskPriority(&task, priority);
  }

  void ProcessAllTasksAtPosition(int32_t max_priority) noexcept {
    for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
      performer_pool_.GetActive(i).ProcessAllTasksAtPosition(max_priority);
    }
  }

  void Update(double duration) noexcept {
    for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
      performer_pool_.GetActive(i).Update(duration);
    }
  }

  [[nodiscard]] PerformerState::TaskKey GetNextTaskKey(double duration) const noexcept {
    PerformerState::TaskKey next_key = {duration, std::numeric_limits<int>::min()};
    for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
      if (const auto maybe_next_key = performer_pool_.GetActive(i).GetNextTaskKey();
          maybe_next_key.has_value() && *maybe_next_key < next_key) {
        next_key = *maybe_next_key;
      }
    }
    return next_key;
  }

  [[nodiscard]] PerformerState& Get(uint32_t performer_index) noexcept {
    return performer_pool_.Get(performer_index);
  }

  [[nodiscard]] const PerformerState& Get(uint32_t performer_index) const noexcept {
    return performer_pool_.Get(performer_index);
  }

  [[nodiscard]] const TaskState& GetTask(uint32_t task_index) const noexcept {
    return task_pool_.Get(task_index);
  }

  [[nodiscard]] bool IsActive(BarelyRef performer) const noexcept {
    return performer_pool_.IsActive(performer.index, performer.generation);
  }

  [[nodiscard]] bool IsActiveTask(BarelyRef task) const noexcept {
    return task_pool_.IsActive(task.index, task.generation);
  }

 private:
  // Performer pool.
  Pool<PerformerState, BARELYMUSICIAN_MAX_PERFORMER_COUNT> performer_pool_;

  // Task pool.
  Pool<TaskState, BARELYMUSICIAN_MAX_TASK_COUNT> task_pool_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
