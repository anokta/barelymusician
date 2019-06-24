#include "barelymusician/message/message_buffer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

void MessageBuffer::Clear() { messages_.clear(); }

void MessageBuffer::Clear(const Iterator& iterator) {
  messages_.erase(iterator.begin, iterator.end);
}

bool MessageBuffer::Empty() const { return messages_.empty(); }

MessageBuffer::Iterator MessageBuffer::GetIterator(int timestamp,
                                                   int num_samples) const {
  Iterator iterator;
  iterator.begin = std::lower_bound(messages_.begin(), messages_.end(),
                                    timestamp, &CompareTimestamp);
  iterator.end = std::lower_bound(iterator.begin, messages_.end(),
                                  timestamp + num_samples, &CompareTimestamp);
  iterator.timestamp = timestamp;
  return iterator;
}

void MessageBuffer::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
