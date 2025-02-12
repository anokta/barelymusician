#ifndef BARELYMUSICIAN_COMMON_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_COMMON_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "common/message.h"

namespace barely {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at a sample.
  ///
  /// @param message_sample Message sample.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(int64_t message_sample, Message message) noexcept;

  /// Returns the next message before an end sample.
  ///
  /// @param end_sample End sample.
  /// @return Pointer to message if successful, `nullptr` otherwise.
  std::pair<int64_t, Message>* GetNext(int64_t end_sample) noexcept;

 private:
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 4096;

  // List of messages with their timestamps in samples.
  std::array<std::pair<int64_t, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_MESSAGE_QUEUE_H_
