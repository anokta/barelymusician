#ifndef BARELYMUSICIAN_ENGINE_TASK_H_
#define BARELYMUSICIAN_ENGINE_TASK_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Task definition alias.
using TaskDefinition = barely::TaskDefinition;

/// Task type alias.
using TaskType = barely::TaskType;

/// Class that wraps task.
class Task {
 public:
  /// Constructs new `Task`.
  ///
  /// @param definition Task definition.
  /// @param user_data Pointer to user data.
  Task(const TaskDefinition& definition, void* user_data) noexcept;

  /// Destroys `Task`.
  ~Task() noexcept;

  /// Non-copyable.
  Task(const Task& other) = delete;
  Task& operator=(const Task& other) = delete;

  /// Movable.
  Task(Task&& other) noexcept = default;
  Task& operator=(Task&& other) noexcept = default;

  /// Processes task.
  void Process() noexcept;

 private:
  // Destroy callback.
  TaskDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  TaskDefinition::ProcessCallback process_callback_;

  // State.
  void* state_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_TASK_H_
