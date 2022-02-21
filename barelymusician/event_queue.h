#ifndef BARELYMUSICIAN_EVENT_QUEUE_H_
#define BARELYMUSICIAN_EVENT_QUEUE_H_

#include <array>
#include <atomic>
#include <utility>

#include "barelymusician/event.h"

namespace barelyapi {

class EventQueue {
 public:
  bool Add(double timestamp, Event event) noexcept;

  bool GetNext(double end_timestamp, std::pair<double, Event>& event) noexcept;

 private:
  // Maximum number of events.
  static constexpr int kMaxNumEvents = 1024;

  // List of events.
  std::array<std::pair<double, Event>, kMaxNumEvents> events_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_EVENT_QUEUE_H_
