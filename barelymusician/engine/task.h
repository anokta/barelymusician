#ifndef BARELYMUSICIAN_ENGINE_TASK_H_
#define BARELYMUSICIAN_ENGINE_TASK_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps a task.
class Task {
 public:
  /// Constructs a new `Task`.
  ///
  /// @param definition Task definition.
  /// @param user_data Pointer to user data.
  Task(const TaskDefinition& definition, void* user_data) noexcept;

  /// Destroys `Task`.
  ~Task() noexcept;

  /// Non-copyable and non-movable.
  Task(const Task& other) noexcept = delete;
  Task& operator=(const Task& other) noexcept = delete;
  Task(Task&& other) noexcept = delete;
  Task& operator=(Task&& other) noexcept = delete;

  /// Processes the task.
  void Process() noexcept;

 private:
  // Destroy callback.
  const TaskDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const TaskDefinition::ProcessCallback process_callback_;

  // State.
  void* state_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_TASK_H_
