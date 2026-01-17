#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>

#include "core/pool.h"
#include "engine/engine_state.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"

namespace barely {

class PerformerController {
 public:
  explicit PerformerController(EngineState& engine) : engine_(engine) {}

  void ProcessAllTasksAtPosition(int32_t max_priority) noexcept;
  void Update(double duration) noexcept;

  [[nodiscard]] uint32_t Acquire() noexcept;
  void Release(uint32_t performer_index) noexcept;

  [[nodiscard]] uint32_t AcquireTask(uint32_t performer_index, double position, double duration,
                                     int32_t priority, BarelyTaskEventCallback callback,
                                     void* user_data) noexcept;
  void ReleaseTask(uint32_t task_index) noexcept;

  void SetLoopBeginPosition(uint32_t performer_index, double loop_begin_position) noexcept {
    auto& performer = engine_.GetPerformer(performer_index);
    if (performer.loop_begin_position == loop_begin_position) {
      return;
    }
    performer.loop_begin_position = loop_begin_position;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetLoopLength(uint32_t performer_index, double loop_length) noexcept {
    auto& performer = engine_.GetPerformer(performer_index);
    if (performer.loop_length == loop_length) {
      return;
    }
    performer.loop_length = loop_length;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetLooping(uint32_t performer_index, bool is_looping) noexcept {
    auto& performer = engine_.GetPerformer(performer_index);
    if (performer.is_looping == is_looping) {
      return;
    }
    performer.is_looping = is_looping;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetPosition(uint32_t performer_index, double position) noexcept {
    auto& performer = engine_.GetPerformer(performer_index);
    if (performer.position == position) {
      return;
    }
    if (performer.is_looping && position >= performer.GetLoopEndPosition()) {
      performer.position = performer.LoopAround(position);
      while (performer.first_active_task_index != UINT32_MAX) {
        SetTaskActive(performer, performer.first_active_task_index, false);
      }
    } else {
      performer.position = position;
      uint32_t task_index = performer.first_active_task_index;
      while (task_index != UINT32_MAX) {
        auto& task = engine_.GetTask(task_index);
        if (task.IsInside(performer.position)) {
          task_index = task.next_task_index;
        } else {
          SetTaskActive(performer, task_index, false);
          // Restart the iteration since links can get invalidated after a callback.
          task_index = performer.first_active_task_index;
        }
      }
    }
  }

  void Start(uint32_t performer_index) noexcept {
    engine_.GetPerformer(performer_index).is_playing = true;
  }

  void Stop(uint32_t performer_index) noexcept {
    auto& performer = engine_.GetPerformer(performer_index);
    performer.is_playing = false;
    while (performer.first_active_task_index != UINT32_MAX) {
      SetTaskActive(performer, performer.first_active_task_index, false);
    }
  }

  void SetTaskDuration(uint32_t task_index, double duration) noexcept {
    assert(duration > 0.0 && "Invalid task duration");
    auto& task = engine_.GetTask(task_index);
    auto& performer = engine_.GetPerformer(task.performer_index);
    if (task.duration == duration) return;
    task.duration = duration;
    if (task.is_active) {
      if (task.IsInside(performer.position)) {
        RemoveTask(performer, task_index);
        InsertActiveTask(performer, task_index);
      } else {
        SetTaskActive(performer, task_index, false);
      }
    }
  }

  void SetTaskEventCallback(uint32_t task_index, BarelyTaskEventCallback callback,
                            void* user_data) noexcept {
    engine_.GetTask(task_index).SetEventCallback(callback, user_data);
  }

  void SetTaskPosition(uint32_t task_index, double position) noexcept {
    auto& task = engine_.GetTask(task_index);
    auto& performer = engine_.GetPerformer(task.performer_index);
    if (task.position == position) return;
    task.position = position;
    if (task.is_active) {
      if (task.IsInside(performer.position)) {
        RemoveTask(performer, task_index);
        InsertActiveTask(performer, task_index);
      } else {
        SetTaskActive(performer, task_index, false);
      }
    } else {
      RemoveTask(performer, task_index);
      InsertInactiveTask(performer, task_index);
    }
  }

  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept {
    auto& task = engine_.GetTask(task_index);
    auto& performer = engine_.GetPerformer(task.performer_index);
    if (task.priority == priority) return;
    task.priority = priority;
    if (task.is_active) {
      RemoveTask(performer, task_index);
      InsertActiveTask(performer, task_index);
    } else {
      RemoveTask(performer, task_index);
      InsertInactiveTask(performer, task_index);
    }
  }

  void GetNextTaskEvent(double& duration, int32_t& priority) const noexcept {
    for (uint32_t i = 0; i < engine_.performer_pool.GetActiveCount(); ++i) {
      GetNextTaskEvent(engine_.performer_pool.GetActive(i), duration, priority);
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
