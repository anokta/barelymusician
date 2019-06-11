#include "barelymusician/message/message_queue.h"

#include <algorithm>

#include "barelymusician/message/message_utils.h"

namespace barelyapi {

void MessageQueue::Clear() { messages_.clear(); }

bool MessageQueue::Pop(int num_samples, Message* message) {
  if (messages_.empty() || messages_.front().timestamp >= num_samples) {
    return false;
  }
  *message = messages_.front();
  messages_.pop_front();
  return true;
}

void MessageQueue::Push(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

void MessageQueue::Update(int num_samples) {
  for (Message& message : messages_) {
    message.timestamp -= num_samples;
  }
}

}  // namespace barelyapi
