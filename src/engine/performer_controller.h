#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>

#include "engine/engine_state.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

class PerformerController {
 public:
  explicit PerformerController(EngineState& engine) : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept;
  void Release(uint32_t performer_index) noexcept;

  [[nodiscard]] uint32_t AcquireTask(uint32_t performer_index, double position, double duration,
                                     int32_t priority, BarelyTaskEventCallback callback,
                                     void* user_data) noexcept;
  void ReleaseTask(uint32_t task_index) noexcept;

  void SetLoopBeginPosition(uint32_t performer_index, double loop_begin_position) noexcept;
  void SetLoopLength(uint32_t performer_index, double loop_length) noexcept;
  void SetLooping(uint32_t performer_index, bool is_looping) noexcept;
  void SetPosition(uint32_t performer_index, double position) noexcept;
  void Start(uint32_t performer_index) noexcept;
  void Stop(uint32_t performer_index) noexcept;

  void SetTaskDuration(uint32_t task_index, double duration) noexcept;
  void SetTaskEventCallback(uint32_t task_index, BarelyTaskEventCallback callback,
                            void* user_data) noexcept;
  void SetTaskPosition(uint32_t task_index, double position) noexcept;
  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept;

  void ProcessAllTasksAtPosition(int32_t max_priority) noexcept;
  void Update(double duration) noexcept;

  void GetNextTaskEvent(double& duration, int32_t& priority) const noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.ActiveCount(); ++i) {
      GetNextTaskEvent(engine_.GetPerformer(engine_.performer_pool.GetActive(i)), duration,
                       priority);
    }
  }

 private:
  void InsertActiveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void InsertInactiveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void RemoveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void SetTaskActive(PerformerState& performer, uint32_t task_index, bool is_active) noexcept;

  [[nodiscard]] uint32_t GetNextInactiveTask(const PerformerState& performer) const noexcept;
  void GetNextTaskEvent(const PerformerState& performer, double& duration,
                        int32_t& priority) const noexcept;

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
