#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_

#include <barelymusician.h>

#include <cassert>
#include <compare>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_set>
#include <utility>

#include "core/callback.h"
#include "core/constants.h"
#include "engine/task_state.h"

namespace barely {

struct PerformerState {
 public:
  /// Task key consists of a position and its priority.
  using TaskKey = std::pair<double, int>;

  // Set of task position-pointer pairs.
  std::set<std::pair<TaskKey, TaskState*>> active_tasks_;
  std::set<std::pair<TaskKey, TaskState*>> inactive_tasks_;

  // Loop begin position in beats.
  double loop_begin_position = 0.0;

  // Loop length in beats.
  double loop_length = 1.0;

  // Position in beats.
  double position = 0.0;

  // Denotes whether performer is looping or not.
  bool is_looping = false;

  // Denotes whether performer is playing or not.
  bool is_playing = false;

  /// Adds a new task.
  ///
  /// @param task Pointer to task.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddTask(TaskState* task) noexcept;

  /// Returns loop end position.
  ///
  /// @return Loop end position in beats.
  [[nodiscard]] double GetLoopEndPosition() const noexcept {
    return loop_begin_position + loop_length;
  }

  /// Returns the next task key.
  ///
  /// @return Optional task key.
  [[nodiscard]] std::optional<TaskKey> GetNextTaskKey() const noexcept;

  /// Processes all tasks at the current position.
  ///
  /// @param max_priority Maximum task priority to process.
  void ProcessAllTasksAtPosition(int max_priority) noexcept;

  /// Removes a task.
  ///
  /// @param task Pointer to task.
  void RemoveTask(TaskState* task) noexcept;

  /// Sets loop begin position.
  ///
  /// @param new_loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double new_loop_begin_position) noexcept;

  /// Sets loop length.
  ///
  /// @param new_loop_length Loop length in beats.
  void SetLoopLength(double new_loop_length) noexcept;

  /// Sets whether performer should be looping or not.
  ///
  /// @param new_is_looping True if looping.
  void SetLooping(bool new_is_looping) noexcept;

  /// Sets position.
  ///
  /// @param new_position Position in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetPosition(double new_position) noexcept;

  /// Sets task duration.
  ///
  /// @param task Pointer to task.
  /// @param new_duration Task duration.
  void SetTaskDuration(TaskState* task, double new_duration) noexcept;

  /// Sets task position.
  ///
  /// @param task Pointer to task.
  /// @param new_position Task position.
  void SetTaskPosition(TaskState* task, double new_position) noexcept;

  /// Sets task priority.
  ///
  /// @param task Pointer to task.
  /// @param new_priority Task priority.
  void SetTaskPriority(TaskState* task, int new_priority) noexcept;

  /// Stops performer.
  void Start() noexcept;

  /// Stops performer.
  void Stop() noexcept;

  /// Updates performer by duration.
  ///
  /// @param duration Duration in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double duration) noexcept;

 private:
  //  Returns an iterator to the next inactive task to process.
  [[nodiscard]] std::set<std::pair<TaskKey, TaskState*>>::const_iterator GetNextInactiveTask()
      const noexcept;

  // Loops around a given `new_position`.
  [[nodiscard]] double LoopAround(double new_position) const noexcept;

  /// Sets the active status of a task.
  void SetTaskActive(const std::set<std::pair<TaskKey, TaskState*>>::iterator& it,
                     bool is_active) noexcept;

  /// Updates the key of an active task.
  void UpdateActiveTaskKey(TaskKey old_task_key, TaskState* task) noexcept;

  /// Updates the key of an inactive task.
  void UpdateInactiveTaskKey(TaskKey old_task_key, TaskState* task) noexcept;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_STATE_H_
