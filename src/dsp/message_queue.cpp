#include "dsp/message_queue.h"

#include <cstdint>
#include <utility>

#include "dsp/message.h"

namespace barely {

bool MessageQueue::Add(int64_t message_sample, Message message) noexcept {
  const int index = write_index_;
  const int next_index = (index + 1) % kMaxMessageCount;
  if (next_index == read_index_) {
    return false;
  }
  messages_[index] = {message_sample, std::move(message)};
  write_index_ = next_index;
  return true;
}

std::pair<int64_t, Message>* MessageQueue::GetNext(int64_t end_sample) noexcept {
  const int index = read_index_;
  if (index == write_index_ || messages_[index].first >= end_sample) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxMessageCount;
  return &messages_[index];
}

}  // namespace barely
