#include "barelymusician/engine/message_queue.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Compares the given message against the given position.
//
// @param message Message.
// @param position position.
// @return True if the message comes prior to the position.
bool CompareMessage(const Message& message, double position) {
  return message.position < position;
}

// Compares the given position against the given message.
//
// @param position position.
// @param message Message.
// @return True if the message comes prior to the position.
bool ComparePosition(double position, const Message& message) {
  return position < message.position;
}

}  // namespace

void MessageQueue::Clear() { messages_.clear(); }

void MessageQueue::Clear(const Iterator& iterator) {
  messages_.erase(iterator.cbegin, iterator.cend);
}

bool MessageQueue::Empty() const { return messages_.empty(); }

MessageQueue::Iterator MessageQueue::GetIterator(double start_position,
                                                 double end_position) const {
  Iterator iterator;
  iterator.cbegin = std::lower_bound(messages_.cbegin(), messages_.cend(),
                                     start_position, &CompareMessage);
  iterator.cend = std::lower_bound(iterator.cbegin, messages_.cend(),
                                   end_position, &CompareMessage);
  return iterator;
}

void MessageQueue::Push(double position, const Message::Data& data) {
  const auto it = std::upper_bound(messages_.cbegin(), messages_.cend(),
                                   position, &ComparePosition);
  messages_.insert(it, {position, data});
}

}  // namespace barelyapi
