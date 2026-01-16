#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "engine/message.h"

namespace barely {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at a frame.
  ///
  /// @param message_frame Message frame.
  /// @param message Message.
  /// @return True if successful, false otherwise.
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

  /// Returns the next message before an end frame.
  ///
  /// @param end_frame End frame.
  /// @return Pointer to message if successful, `nullptr` otherwise.
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
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 8192;

  // List of messages with their timestamps in frames.
  std::array<std::pair<int64_t, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
