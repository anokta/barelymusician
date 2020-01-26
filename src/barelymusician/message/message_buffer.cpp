#include "barelymusician/message/message_buffer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

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

// Compares the given message against the given timestamp.
//
// @param message Message.
// @param timestamp Timestamp.
// @return True if the message comes prior to the timestamp.
bool CompareTimestamp(const Message& message, int timestamp) {
  return message.timestamp < timestamp;
}

}  // namespace

void MessageBuffer::Clear() { messages_.clear(); }

void MessageBuffer::Clear(const Iterator& iterator) {
  messages_.erase(iterator.cbegin, iterator.cend);
}

bool MessageBuffer::Empty() const { return messages_.empty(); }

MessageBuffer::Iterator MessageBuffer::GetIterator(int timestamp,
                                                   int num_samples) const {
  Iterator iterator;
  iterator.cbegin = std::lower_bound(messages_.cbegin(), messages_.cend(),
                                     timestamp, &CompareTimestamp);
  iterator.cend = std::lower_bound(iterator.cbegin, messages_.cend(),
                                   timestamp + num_samples, &CompareTimestamp);
  iterator.timestamp = timestamp;
  return iterator;
}

void MessageBuffer::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.cbegin(), messages_.cend(),
                                   message, &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
