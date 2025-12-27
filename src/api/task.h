#ifndef BARELYMUSICIAN_API_TASK_H_
#define BARELYMUSICIAN_API_TASK_H_

#include <barelymusician.h>

#include "common/callback.h"
#include "common/constants.h"

/// Implementation of a task.
struct BarelyTask {
  /// Event callback alias.
  using EventCallback = barely::Callback<BarelyTaskEventCallback>;

  /// Event callback.
  EventCallback event_callback = {};

  /// Position in beats.
  double position = 0.0;

  /// Duration in beats.
  double duration = 0.0;

  /// Priority.
  int priority = 0;

  /// Performer index.
  uint32_t performer_index = 0;

  /// Denotes whether the task is active or not.
  bool is_active = false;

  /// Returns the end position.
  ///
  /// @return End position in beats.
  double GetEndPosition() const noexcept { return position + duration; }

  /// Returns whether a position is inside the task boundaries.
  ///
  /// @param position Position in beats.
  /// @return True if inside, false otherwise.
  bool IsInside(double other_position) const noexcept {
    return other_position >= position && other_position < GetEndPosition();
  }

  /// Processes the task.
  ///
  /// @param type Task event type.
  void Process(BarelyTaskEventType type) noexcept { event_callback(type); }

  /// Sets whether the task is currently active or not.
  ///
  /// @param new_is_active True if active, false otherwise.
  void SetActive(bool new_is_active) noexcept {
    is_active = new_is_active;
    Process(is_active ? BarelyTaskEventType_kBegin : BarelyTaskEventType_kEnd);
  }

  /// Sets the event callback.
  ///
  /// @param new_event_callback Event callback.
  void SetEventCallback(EventCallback new_event_callback) noexcept {
    if (is_active) {
      Process(BarelyTaskEventType_kEnd);
    }
    event_callback = new_event_callback;
    if (is_active) {
      Process(BarelyTaskEventType_kBegin);
    }
  }
};

#endif  // BARELYMUSICIAN_API_TASK_H_
