#ifndef BARELYMUSICIAN_DSP_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_DSP_MESSAGE_QUEUE_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

#include "dsp/message.h"

namespace barely {

/// Single-consumer single-producer message queue.
class MessageQueue {
 public:
  /// Adds a message at a frame.
  ///
  /// @param message_frame Message frame.
  /// @param message Message.
  /// @return True if successful, false otherwise.
  bool Add(int64_t message_frame, Message message) noexcept;

  /// Returns the next message before an end frame.
  ///
  /// @param end_frame End frame.
  /// @return Pointer to message if successful, `nullptr` otherwise.
  std::pair<int64_t, Message>* GetNext(int64_t end_frame) noexcept;

 private:
  // Maximum number of messages.
  static constexpr int kMaxMessageCount = 4096;

  // List of messages with their timestamps in frames.
  std::array<std::pair<int64_t, Message>, kMaxMessageCount> messages_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_MESSAGE_QUEUE_H_
