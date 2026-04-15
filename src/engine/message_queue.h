#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "core/arena.h"
#include "engine/message.h"

namespace barely {

inline constexpr size_t kMaxMessageCount = 8192;

// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  void Init(Arena& arena) noexcept {
    messages_ = arena.AllocArray<std::pair<int64_t, Message>>(kMaxMessageCount);
  }

  bool Add(int64_t message_frame, Message message) noexcept {
    const int index = write_index_.load(std::memory_order_relaxed);
    const int next_index = (index + 1) % kMaxMessageCount;
    if (next_index == read_index_.load(std::memory_order_acquire)) {
      return false;
    }
    messages_[index] = {message_frame, std::move(message)};
    write_index_.store(next_index, std::memory_order_release);
    return true;
  }

  std::pair<int64_t, Message>* GetNext(int64_t end_frame) noexcept {
    const int index = read_index_.load(std::memory_order_relaxed);
    if (index == write_index_.load(std::memory_order_acquire) ||
        messages_[index].first >= end_frame) {
      return nullptr;
    }
    read_index_.store((index + 1) % kMaxMessageCount, std::memory_order_release);
    return &messages_[index];
  }

 private:
  // Array of messages with their timestamps in frames.
  std::pair<int64_t, Message>* messages_ = nullptr;

  std::atomic<int> read_index_ = 0;
  std::atomic<int> write_index_ = 0;
};

inline constexpr size_t GetMessageQueueSize() noexcept {
  return kMaxMessageCount * sizeof(std::pair<int64_t, Message>) +
         alignof(std::pair<int64_t, Message>);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
