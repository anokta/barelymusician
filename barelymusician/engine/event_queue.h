#ifndef BARELYMUSICIAN_ENGINE_EVENT_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_EVENT_QUEUE_H_

#include <array>
#include <atomic>
#include <utility>

#include "barelymusician/engine/event.h"

namespace barelyapi {

/// Single-consumer single-producer event queue.
class EventQueue {
 public:
  /// Adds event at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param event Event.
  /// @return True if successful, false otherwise.
  bool Add(double timestamp, Event event) noexcept;

  /// Returns next event before end timestamp.
  ///
  /// @param end_timestamp End timestamp in seconds.
  /// @return Pointer to event if successful, nullptr otherwise.
  std::pair<double, Event>* GetNext(double end_timestamp) noexcept;

 private:
  // Maximum number of events.
  static constexpr int kMaxNumEvents = 1024;

  // List of events with their timestamps.
  std::array<std::pair<double, Event>, kMaxNumEvents> events_;

  // Read index.
  std::atomic<int> read_index_ = 0;

  // Write index.
  std::atomic<int> write_index_ = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_EVENT_QUEUE_H_
