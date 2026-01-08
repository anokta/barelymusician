#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>
#include <optional>

#include "core/pool.h"
#include "engine/engine_state.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

class PerformerController {
 public:
  explicit PerformerController(EngineState& engine) : engine_(engine) {}

  [[nodiscard]] BarelyRef Acquire() noexcept;
  void Release(uint32_t performer_index) noexcept;

  [[nodiscard]] BarelyRef AcquireTask(uint32_t performer_index, double position, double duration,
                                      int32_t priority, BarelyTaskEventCallback callback,
                                      void* user_data) noexcept;
  void ReleaseTask(uint32_t task_index) noexcept;

  void SetTaskDuration(uint32_t task_index, double duration) noexcept {
    auto& task = engine_.task_pool.Get(task_index);
    engine_.performer_pool.Get(task.performer_index).SetTaskDuration(&task, duration);
  }
  void SetTaskEventCallback(uint32_t task_index, BarelyTaskEventCallback callback,
                            void* user_data) noexcept {
    engine_.task_pool.Get(task_index).SetEventCallback(callback, user_data);
  }
  void SetTaskPosition(uint32_t task_index, double position) noexcept {
    auto& task = engine_.task_pool.Get(task_index);
    engine_.performer_pool.Get(task.performer_index).SetTaskPosition(&task, position);
  }
  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept {
    auto& task = engine_.task_pool.Get(task_index);
    engine_.performer_pool.Get(task.performer_index).SetTaskPriority(&task, priority);
  }

  void ProcessAllTasksAtPosition(int32_t max_priority) noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
      engine_.performer_pool.GetActive(i).ProcessAllTasksAtPosition(max_priority);
    }
  }

  void Update(double duration) noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
      engine_.performer_pool.GetActive(i).Update(duration);
    }
  }

  [[nodiscard]] PerformerState::TaskKey GetNextTaskKey(double duration) const noexcept {
    PerformerState::TaskKey next_key = {duration, INT32_MIN};
    for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
      if (const auto maybe_next_key = engine_.performer_pool.GetActive(i).GetNextTaskKey();
          maybe_next_key.has_value() && *maybe_next_key < next_key) {
        next_key = *maybe_next_key;
      }
    }
    return next_key;
  }

 private:
  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
