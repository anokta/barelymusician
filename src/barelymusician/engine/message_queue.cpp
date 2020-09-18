#include "barelymusician/engine/message_queue.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Compares the given message against the given timestamp.
//
// @param message Message.
// @param timestamp Timestamp.
// @return True if the message comes prior to the timestamp.
bool CompareMessage(const Message& message, double timestamp) {
  return message.timestamp < timestamp;
}

// Compares the given timestamp against the given message.
//
// @param timestamp Timestamp.
// @param message Message.
// @return True if the message comes prior to the timestamp.
bool CompareTimestamp(double timestamp, const Message& message) {
  return timestamp < message.timestamp;
}

}  // namespace

void MessageQueue::Clear() { messages_.clear(); }

void MessageQueue::Clear(const Iterator& iterator) {
  messages_.erase(iterator.cbegin, iterator.cend);
}

bool MessageQueue::Empty() const { return messages_.empty(); }

MessageQueue::Iterator MessageQueue::GetIterator(double begin_timestamp,
                                                 double end_timestamp) const {
  Iterator iterator;
  iterator.cbegin = std::lower_bound(messages_.cbegin(), messages_.cend(),
                                     begin_timestamp, &CompareMessage);
  iterator.cend = std::lower_bound(iterator.cbegin, messages_.cend(),
                                   end_timestamp, &CompareMessage);
  return iterator;
}

void MessageQueue::Push(double timestamp, const Message::Data& data) {
  const auto it = std::upper_bound(messages_.cbegin(), messages_.cend(),
                                   timestamp, &CompareTimestamp);
  messages_.insert(it, {timestamp, data});
}

}  // namespace barelyapi
