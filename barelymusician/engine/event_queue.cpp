#include "barelymusician/engine/event_queue.h"

#include <cassert>
#include <utility>

namespace barely::internal {

bool EventQueue::Add(double timestamp, Event event) noexcept {
  assert(timestamp >= 0.0);
  const int index = write_index_;
  const int next_index = (index + 1) % kMaxNumEvents;
  if (next_index == read_index_) {
    return false;
  }
  events_[index] = {timestamp, std::move(event)};
  write_index_ = next_index;
  return true;
}

std::pair<double, Event>* EventQueue::GetNext(double end_timestamp) noexcept {
  assert(end_timestamp >= 0.0);
  const int index = read_index_;
  if (index == write_index_ || events_[index].first >= end_timestamp) {
    return nullptr;
  }
  read_index_ = (index + 1) % kMaxNumEvents;
  return &events_[index];
}

}  // namespace barely::internal
