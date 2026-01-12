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

  [[nodiscard]] TaskKey GetNextTaskKey(double duration) const noexcept;

  void SetLoopBeginPosition(uint32_t performer_index, double loop_begin_position) noexcept {
    auto& performer = engine_.performer_pool.Get(performer_index);
    if (performer.loop_begin_position == loop_begin_position) {
      return;
    }
    performer.loop_begin_position = loop_begin_position;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetLoopLength(uint32_t performer_index, double loop_length) noexcept {
    auto& performer = engine_.performer_pool.Get(performer_index);
    if (performer.loop_length == loop_length) {
      return;
    }
    performer.loop_length = loop_length;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetLooping(uint32_t performer_index, bool is_looping) noexcept {
    auto& performer = engine_.performer_pool.Get(performer_index);
    if (performer.is_looping == is_looping) {
      return;
    }
    performer.is_looping = is_looping;
    if (performer.is_looping && performer.position >= performer.GetLoopEndPosition()) {
      SetPosition(performer_index, performer.LoopAround(performer.position));
    }
  }

  void SetPosition(uint32_t performer_index, double position) noexcept {
    auto& performer = engine_.performer_pool.Get(performer_index);
    if (performer.position == position) {
      return;
    }
    if (performer.is_looping && position >= performer.GetLoopEndPosition()) {
      performer.position = performer.LoopAround(position);
      while (!performer.active_tasks.empty()) {
        SetTaskActive(performer, performer.active_tasks.begin(), false);
      }
    } else {
      performer.position = position;
      for (auto it = performer.active_tasks.begin(); it != performer.active_tasks.end();) {
        // Copy the values in case `it` gets invalidated after the `Process` call.
        auto [end_position, task_index] = *it;
        if (!engine_.task_pool.Get(task_index).IsInside(performer.position)) {
          SetTaskActive(performer, it, false);
        }
        it = performer.active_tasks.upper_bound({end_position, task_index});
      }
    }
  }

  void Start(uint32_t performer_index) noexcept {
    engine_.performer_pool.Get(performer_index).is_playing = true;
  }

  void Stop(uint32_t performer_index) noexcept {
    auto& performer = engine_.performer_pool.Get(performer_index);
    performer.is_playing = false;
    while (!performer.active_tasks.empty()) {
      SetTaskActive(performer, performer.active_tasks.begin(), false);
    }
  }

  void SetTaskDuration(uint32_t task_index, double duration) noexcept {
    assert(duration > 0.0 && "Invalid task duration");
    auto& task = engine_.task_pool.Get(task_index);
    auto& performer = engine_.performer_pool.Get(task.performer_index);
    if (task.duration == duration) return;
    const double old_duration = task.duration;
    task.duration = duration;
    if (task.is_active) {
      const TaskKey old_task_key = {task.position + old_duration, task.priority};
      if (task.IsInside(performer.position)) {
        UpdateActiveTaskKey(performer, old_task_key, task_index);
      } else {
        SetTaskActive(performer, performer.active_tasks.find({old_task_key, task_index}), false);
      }
    }
  }

  void SetTaskEventCallback(uint32_t task_index, BarelyTaskEventCallback callback,
                            void* user_data) noexcept {
    engine_.task_pool.Get(task_index).SetEventCallback(callback, user_data);
  }

  void SetTaskPosition(uint32_t task_index, double position) noexcept {
    auto& task = engine_.task_pool.Get(task_index);
    auto& performer = engine_.performer_pool.Get(task.performer_index);
    if (task.position == position) return;
    const double old_position = task.position;
    task.position = position;
    if (task.is_active) {
      const TaskKey old_task_key = {old_position + task.duration, task.priority};
      if (task.IsInside(performer.position)) {
        UpdateActiveTaskKey(performer, old_task_key, task_index);
      } else {
        SetTaskActive(performer, performer.active_tasks.find({old_task_key, task_index}), false);
      }
    } else {
      UpdateInactiveTaskKey(performer, {old_position, task.priority}, task_index);
    }
  }

  void SetTaskPriority(uint32_t task_index, int32_t priority) noexcept {
    auto& task = engine_.task_pool.Get(task_index);
    auto& performer = engine_.performer_pool.Get(task.performer_index);
    if (task.priority == priority) return;
    const int32_t old_priority = task.priority;
    task.priority = priority;
    const TaskKey old_task_key = {task.position, old_priority};
    if (task.is_active) {
      UpdateActiveTaskKey(performer, old_task_key, task_index);
    } else {
      UpdateInactiveTaskKey(performer, old_task_key, task_index);
    }
  }

 private:
  void UpdateNextKey(const PerformerState& performer, TaskKey& next_key) const noexcept;

  void SetTaskActive(PerformerState& performer,
                     const std::set<std::pair<TaskKey, uint32_t>>::iterator& it,
                     bool is_active) noexcept {
    const uint32_t task_index = it->second;
    auto& task = engine_.task_pool.Get(task_index);
    auto node = (is_active ? performer.inactive_tasks : performer.active_tasks).extract(it);
    node.value().first.first = is_active ? task.GetEndPosition() : task.position;
    (is_active ? performer.active_tasks : performer.inactive_tasks).insert(std::move(node));
    task.SetActive(is_active);
  }

  void UpdateActiveTaskKey(PerformerState& performer, TaskKey old_task_key,
                           uint32_t task_index) noexcept {
    auto node = performer.active_tasks.extract({old_task_key, task_index});
    auto& task = engine_.task_pool.Get(task_index);
    node.value().first = {task.GetEndPosition(), task.priority};
    performer.active_tasks.insert(std::move(node));
  }

  void UpdateInactiveTaskKey(PerformerState& performer, TaskKey old_task_key,
                             uint32_t task_index) noexcept {
    auto node = performer.inactive_tasks.extract({old_task_key, task_index});
    auto& task = engine_.task_pool.Get(task_index);
    node.value().first = {task.position, task.priority};
    performer.inactive_tasks.insert(std::move(node));
  }

  [[nodiscard]] std::set<std::pair<TaskKey, uint32_t>>::const_iterator GetNextInactiveTask(
      const PerformerState& performer) const noexcept {
    if (!performer.is_playing) {
      return performer.inactive_tasks.end();
    }
    auto next_it = performer.inactive_tasks.lower_bound({{performer.position, INT32_MIN}, 0});
    // Check if any inactive task became active (in case a new position was set).
    // TODO(#147): This may be optimized further using an interval tree.
    for (auto it = performer.inactive_tasks.begin(); it != next_it; ++it) {
      if (engine_.task_pool.Get(it->second).GetEndPosition() > performer.position) {
        return it;
      }
    }
    return next_it;
  }

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_CONTROLLER_H_
