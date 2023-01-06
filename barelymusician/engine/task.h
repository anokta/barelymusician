#ifndef BARELYMUSICIAN_ENGINE_TASK_H_
#define BARELYMUSICIAN_ENGINE_TASK_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Task definition alias.
using TaskDefinition = barely::TaskDefinition;

/// Class that wraps task.
class Task {
 public:
  /// Constructs new `Task`.
  ///
  /// @param definition Task definition.
  explicit Task(const TaskDefinition& definition,
                void* user_data = nullptr) noexcept;

  /// Destroys `Task`.
  ~Task() noexcept;

  /// Non-copyable and non-movable.
  Task(const Task& other) = delete;
  Task& operator=(const Task& other) = delete;
  Task(Task&& other) noexcept = delete;
  Task& operator=(Task&& other) noexcept = delete;

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
