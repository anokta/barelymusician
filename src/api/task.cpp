#include "api/task.h"

#include <barelymusician.h>

#include <cassert>

#include "api/performer.h"

void BarelyTask::SetDuration(double new_duration) noexcept {
  assert(new_duration > 0.0 && "Invalid task duration");
  if (new_duration != duration) {
    const double old_duration = duration;
    duration = new_duration;
    performer->SetTaskDuration(this, old_duration);
  }
}

void BarelyTask::SetEventCallback(EventCallback new_event_callback) noexcept {
  if (is_active) {
    Process(BarelyTaskEventType_kEnd);
  }
  event_callback = new_event_callback;
  if (is_active) {
    Process(BarelyTaskEventType_kBegin);
  }
}

void BarelyTask::SetPosition(double new_position) noexcept {
  if (new_position != position) {
    const double old_position = position;
    position = new_position;
    performer->SetTaskPosition(this, old_position);
  }
}

void BarelyTask::SetPriority(int new_priority) noexcept {
  if (new_priority != priority) {
    const int old_priority = priority;
    priority = new_priority;
    performer->SetTaskPriority(this, old_priority);
  }

}  // namespace barely
