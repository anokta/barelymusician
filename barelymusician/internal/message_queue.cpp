#include "barelymusician/internal/message_queue.h"

#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/internal/message.h"

namespace barely::internal {

bool MessageQueue::Add(Rational timestamp, Message message) noexcept {
  const int index = write_index_;
  const int next_index = (index + 1) % kMaxMessageCount;
  if (next_index == read_index_) {
    return false;
  }
  messages_[index] = {timestamp, std::move(message)};
  write_index_ = next_index;
  return true;
}

std::pair<Rational, Message>* MessageQueue::GetNext(Rational end_timestamp) noexcept {
  const int index = read_index_;
  if (index == write_index_ || messages_[index].first >= end_timestamp) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxMessageCount;
  return &messages_[index];
}

}  // namespace barely::internal
