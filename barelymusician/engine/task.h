#ifndef BARELYMUSICIAN_ENGINE_TASK_H_
#define BARELYMUSICIAN_ENGINE_TASK_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps task.
class Task {
 public:
  /// Definition alias.
  using Definition = barely::TaskDefinition;

  /// Constructs new `Task`.
  ///
  /// @param definition Task definition.
  explicit Task(const Definition& definition,
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
  Definition::DestroyCallback destroy_callback_;

  // Process callback.
  Definition::ProcessCallback process_callback_;

  // State.
  void* state_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_TASK_H_
