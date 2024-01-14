#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "barelymusician/internal/message.h"

namespace barely::internal {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at timestamp.
  ///
  /// @param timestamp Timestamp in frames.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(std::int64_t timestamp, Message message) noexcept;

  /// Returns the next message before an end timestamp.
  ///
  /// @param end_timestamp End Timestamp in frames.
  /// @return Pointer to message if successful, `nullptr` otherwise.
  std::pair<std::int64_t, Message>* GetNext(std::int64_t end_timestamp) noexcept;

 private:
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 4096;

  // List of messages with their timestamps.
  std::array<std::pair<std::int64_t, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
