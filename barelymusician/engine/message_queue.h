#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <utility>

#include "barelymusician/engine/message.h"
#include "barelymusician/engine/number.h"

namespace barely::internal {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds message at timestamp.
  ///
  /// @param timestamp Timestamp in nanoseconds.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(Integer timestamp, Message message) noexcept;

  /// Returns next message before end timestamp.
  ///
  /// @param end_timestamp End timestamp in nanoseconds.
  /// @return Pointer to message if successful, nullptr otherwise.
  std::pair<Integer, Message>* GetNext(Integer end_timestamp) noexcept;

 private:
  // Maximum number of messages.
  static constexpr Integer kMaxMessageCount = 1024;

  // List of messages with their timestamps.
  std::array<std::pair<Integer, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<Integer> read_index_ = 0;

  // Write index.
  std::atomic<Integer> write_index_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
