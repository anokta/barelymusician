#include "barelymusician/engine/message_buffer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Compares the given two messages with respect to their positions.
//
// @param lhs First message.
// @param rhs Second message.
// @return True if the first message comes prior to the second message.
bool CompareMessage(const Message& lhs, const Message& rhs) {
  return lhs.position < rhs.position;
}

// Compares the given message against the given position.
//
// @param message Message.
// @param position position.
// @return True if the message comes prior to the position.
bool ComparePosition(const Message& message, double position) {
  return message.position < position;
}

}  // namespace

void MessageBuffer::Clear() { messages_.clear(); }

void MessageBuffer::Clear(const Iterator& iterator) {
  messages_.erase(iterator.cbegin, iterator.cend);
}

bool MessageBuffer::Empty() const { return messages_.empty(); }

MessageBuffer::Iterator MessageBuffer::GetIterator(double start_position,
                                                   double end_position) const {
  Iterator iterator;
  iterator.cbegin = std::lower_bound(messages_.cbegin(), messages_.cend(),
                                     start_position, &ComparePosition);
  iterator.cend = std::lower_bound(iterator.cbegin, messages_.cend(),
                                   end_position, &ComparePosition);
  return iterator;
}

void MessageBuffer::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.cbegin(), messages_.cend(),
                                   message, &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
