#include "barelymusician/engine/message_queue.h"

#include <cassert>
#include <utility>

namespace barely::internal {

bool MessageQueue::Add(double timestamp, Message message) noexcept {
  assert(timestamp >= 0.0);
  const int index = write_index_;
  const int next_index = (index + 1) % kMaxNumMessages;
  if (next_index == read_index_) {
    return false;
  }
  messages_[index] = {timestamp, std::move(message)};
  write_index_ = next_index;
  return true;
}

std::pair<double, Message>* MessageQueue::GetNext(
    double end_timestamp) noexcept {
  assert(end_timestamp >= 0.0);
  const int index = read_index_;
  if (index == write_index_ || messages_[index].first >= end_timestamp) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxNumMessages;
  return &messages_[index];
}

}  // namespace barely::internal
