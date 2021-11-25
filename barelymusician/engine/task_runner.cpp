#include "barelymusician/engine/task_runner.h"

#include <atomic>
#include <cassert>
#include <utility>

namespace barely {

TaskRunner::TaskRunner(int max_size) : nodes_(max_size) {
  assert(max_size > 0);
  for (int i = 0; i < max_size - 1; ++i) {
    nodes_[i].next = &nodes_[i + 1];
  }
  nodes_[max_size - 1].next = nullptr;

  active_head_ = nullptr;
  free_head_ = &nodes_[0];
  temp_tasks_.reserve(max_size);
}

void TaskRunner::Add(Task&& task) {
  if (Node* node = PopNode(&free_head_)) {
    node->task = std::move(task);
    PushNode(&active_head_, node);
  }
}

void TaskRunner::Run() {
  // Iterate through the stacked tasks.
  Node* it = active_head_.exchange(nullptr);
  while (it) {
    Node* const next = it->next;
    temp_tasks_.emplace_back(std::move(it->task));
    it->task = nullptr;
    PushNode(&free_head_, it);
    it = next;
  }
  // Execute tasks in reverse order.
  for (auto rit = temp_tasks_.rbegin(); rit != temp_tasks_.rend(); ++rit) {
    if (*rit) {
      (*rit)();
    }
  }
  temp_tasks_.clear();
}

TaskRunner::Node* TaskRunner::PopNode(std::atomic<Node*>* head) {
  Node* old_head = nullptr;
  Node* old_head_next = nullptr;
  do {
    old_head = head->load();
    if (!old_head) {
      return nullptr;
    }
    old_head_next = old_head->next.load();
  } while (!head->compare_exchange_weak(old_head, old_head_next,
                                        std::memory_order_relaxed,
                                        std::memory_order_relaxed));
  return old_head;
}

void TaskRunner::PushNode(std::atomic<Node*>* head, Node* node) {
  Node* old_head = nullptr;
  do {
    old_head = head->load();
    node->next = old_head;
  } while (!head->compare_exchange_weak(
      old_head, node, std::memory_order_release, std::memory_order_relaxed));
}

}  // namespace barely
