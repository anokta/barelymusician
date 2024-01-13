#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/message.h"

namespace barely::internal {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(Rational timestamp, Message message) noexcept;

  /// Returns the next message before an end timestamp.
  ///
  /// @param end_timestamp End timestamp in seconds.
  /// @return Pointer to message if successful, `nullptr` otherwise.
  std::pair<Rational, Message>* GetNext(Rational end_timestamp) noexcept;

 private:
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 4096;

  // List of messages with their timestamps.
  std::array<std::pair<Rational, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_QUEUE_H_
