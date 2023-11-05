#include "barelymusician/internal/message_queue.h"

#include <utility>

namespace barely::internal {

bool MessageQueue::Add(double timestamp, Message message) noexcept {
  const int index = write_index_;
  const int next_index = (index + 1) % kMaxMessageCount;
  if (next_index == read_index_) {
    return false;
  }
  messages_[index] = {timestamp, std::move(message)};
  write_index_ = next_index;
  return true;
}

std::pair<double, Message>* MessageQueue::GetNext(double end_timestamp) noexcept {
  const int index = read_index_;
  if (index == write_index_ || messages_[index].first >= end_timestamp) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxMessageCount;
  return &messages_[index];
}

}  // namespace barely::internal
