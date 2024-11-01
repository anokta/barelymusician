#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "internal/message.h"

namespace barely::internal {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at a frame.
  ///
  /// @param frame Message frame.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(int64_t frame, Message message) noexcept;

  /// Returns the next message before an end frame.
  ///
  /// @param end_frame End frame.
  /// @return Pointer to message if successful, `nullptr` otherwise.
  std::pair<int64_t, Message>* GetNext(int64_t end_frame) noexcept;

 private:
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 4096;

  // List of messages with their frames.
  std::array<std::pair<int64_t, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
