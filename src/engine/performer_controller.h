#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>
#include <optional>

#include "engine/engine_state.h"
#include "engine/performer_state.h"

namespace barely {

class PerformerController {
 public:
  explicit PerformerController(EngineState& engine) : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept;
  void Release(uint32_t performer_index) noexcept;

  [[nodiscard]] uint32_t AcquireTask(uint32_t performer_index, double position, double duration,
                                     int32_t priority, BarelyTaskCallback callback,
                                     void* user_data) noexcept;
  void ReleaseTask(uint32_t task_index) noexcept;

  void SetLoopBeginPosition(uint32_t performer_index, double loop_begin_position) noexcept;
  void SetLoopLength(uint32_t performer_index, double loop_length) noexcept;
  void SetLooping(uint32_t performer_index, bool is_looping) noexcept;
  void SetPosition(uint32_t performer_index, double position) noexcept;
  void Start(uint32_t performer_index) noexcept;
  void Stop(uint32_t performer_index) noexcept;

  void SetTaskDuration(uint32_t task_index, double duration) noexcept;
  void SetTaskCallback(uint32_t task_index, BarelyTaskCallback callback, void* user_data) noexcept;
  void SetTaskPosition(uint32_t task_index, double position) noexcept;
  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept;

  void ProcessAllTasksAtPosition(const std::optional<int32_t>& min_priority,
                                 int32_t max_priority) noexcept;

  void UpdatePosition(double duration) noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.ActiveCount(); ++i) {
      const uint32_t performer_index = engine_.performer_pool.GetActive(i);
      if (const auto& performer = engine_.GetPerformer(performer_index); performer.is_playing) {
        SetPosition(performer_index, performer.position + duration);
      }
    }
  }

  void GetNextTaskEvent(const std::optional<int32_t>& min_priority, double& duration,
                        int32_t& priority) const noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.ActiveCount(); ++i) {
      GetNextTaskEvent(engine_.GetPerformer(engine_.performer_pool.GetActive(i)), min_priority,
                       duration, priority);
    }
  }

 private:
  void InsertActiveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void InsertInactiveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void RemoveTask(PerformerState& performer, uint32_t task_index) noexcept;
  void SetTaskActive(PerformerState& performer, uint32_t task_index, bool is_active) noexcept;
  void UpdateActiveTasks(PerformerState& performer) noexcept;

  [[nodiscard]] uint32_t GetNextInactiveTask(const PerformerState& performer) const noexcept;
  void GetNextTaskEvent(const PerformerState& performer, const std::optional<int32_t>& min_priority,
                        double& duration, int32_t& priority) const noexcept;

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
