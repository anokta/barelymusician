#include "barelymusician/engine/message_queue.h"

#include <cassert>
#include <utility>

#include "barelymusician/engine/number.h"

namespace barely::internal {

bool MessageQueue::Add(Integer timestamp, Message message) noexcept {
  assert(timestamp >= 0);
  const Integer index = write_index_;
  const Integer next_index = (index + 1) % kMaxMessageCount;
  if (next_index == read_index_) {
    return false;
  }
  messages_[index] = {timestamp, std::move(message)};
  write_index_ = next_index;
  return true;
}

std::pair<Integer, Message>* MessageQueue::GetNext(
    Integer end_timestamp) noexcept {
  assert(end_timestamp >= 0.0);
  const Integer index = read_index_;
  if (index == write_index_ || messages_[index].first >= end_timestamp) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxMessageCount;
  return &messages_[index];
}

}  // namespace barely::internal
