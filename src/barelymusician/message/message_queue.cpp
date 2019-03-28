#include "barelymusician/message/message_queue.h"

#include <algorithm>

namespace barelyapi {

namespace {

// Compares the given two messages with respect to their timestamps.
//
// @param lhs First message.
// @param rhs Second message.
// @return True if the first message comes prior to the second message.
bool CompareMessage(const Message& lhs, const Message& rhs) {
  return lhs.timestamp < rhs.timestamp;
}

// Compares the given |message| against the given |timestamp|.
//
// @param message Message.
// @param timestamp Timestamp.
// @return True if the message comes prior to the timestamp.
bool CompareTimestamp(const Message& message, int timestamp) {
  return message.timestamp < timestamp;
}

}  // namespace

std::vector<Message> MessageQueue::Pop(int start_sample, int num_samples) {
  const auto begin = std::lower_bound(messages_.begin(), messages_.end(),
                                      start_sample, &CompareTimestamp);
  if (begin == messages_.end()) {
    // No messages found within the range, return an empty list.
    return std::vector<Message>();
  }

  const int end_sample = start_sample + num_samples;
  const auto end =
      std::lower_bound(begin, messages_.end(), end_sample, &CompareTimestamp);

  std::vector<Message> messages_in_range(begin, end);
  messages_.erase(begin, end);
  return messages_in_range;
}

void MessageQueue::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

void MessageQueue::Reset() { messages_.clear(); }

}  // namespace barelyapi
