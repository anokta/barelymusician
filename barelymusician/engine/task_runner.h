#ifndef BARELYMUSICIAN_ENGINE_TASK_RUNNER_H_
#define BARELYMUSICIAN_ENGINE_TASK_RUNNER_H_

#include <atomic>
#include <functional>
#include <vector>

namespace barely {

/// Lock-free thread-safe task runner (single producer, single consumer).
class TaskRunner {
 public:
  /// Task signature.
  using Task = std::function<void()>;

  /// Constructs new |TaskRunner| with the given |max_size|.
  ///
  /// @param max_size Maximum number of tasks that can be added in each run.
  explicit TaskRunner(int max_size);

  /// Adds new task.
  ///
  /// @param task Task to run.
  void Add(Task&& task);

  /// Runs all tasks.
  void Run();

 private:
  // Single-linked list node.
  struct Node {
    // Task.
    Task task;

    // Pointer to next node.
    std::atomic<Node*> next;
  };

  // Pops and returns node from the given |head|.
  Node* PopNode(std::atomic<Node*>* head);

  // Pushes the given |node| to the given |head|.
  void PushNode(std::atomic<Node*>* head, Node* node);

  // Active list head node.
  std::atomic<Node*> active_head_;

  // Free list head node.
  std::atomic<Node*> free_head_;

  // Pre-allocated node list.
  std::vector<Node> nodes_;

  // Temporary list to run the stacked tasks in FIFO order.
  // TODO(#50): This would probably be unnecessary by switching to a queue?
  std::vector<Task> temp_tasks_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TASK_RUNNER_H_
