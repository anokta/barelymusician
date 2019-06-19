#include "barelymusician/message/message_buffer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

void MessageBuffer::Clear() { messages_.clear(); }

bool MessageBuffer::Pop(int num_samples, Message* message) {
  DCHECK(message);
  if (messages_.empty() || messages_.front().timestamp >= num_samples) {
    return false;
  }
  *message = messages_.front();
  messages_.pop_front();
  return true;
}

void MessageBuffer::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

void MessageBuffer::Update(int num_samples) {
  for (Message& message : messages_) {
    message.timestamp -= num_samples;
  }
}

}  // namespace barelyapi
