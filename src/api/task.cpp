#include "api/task.h"

#include <barelymusician.h>

#include <cassert>

#include "api/performer.h"

BarelyTask::BarelyTask(BarelyPerformer& performer, double position, double duration, int priority,
                       EventCallback callback) noexcept
    : performer_(performer),
      position_(position),
      duration_(duration),
      event_callback_(callback),
      priority_(priority) {
  assert(duration > 0.0 && "Invalid task duration");
  performer_.AddTask(this);
}

BarelyTask::~BarelyTask() noexcept { performer_.RemoveTask(this); }

void BarelyTask::SetDuration(double duration) noexcept {
  assert(duration > 0.0 && "Invalid task duration");
  if (duration != duration_) {
    const double old_duration = duration_;
    duration_ = duration;
    performer_.SetTaskDuration(this, old_duration);
  }
}

void BarelyTask::SetEventCallback(EventCallback callback) noexcept {
  if (is_active_) {
    Process(BarelyTaskEventType_kEnd);
  }
  event_callback_ = callback;
  if (is_active_) {
    Process(BarelyTaskEventType_kBegin);
  }
}

void BarelyTask::SetPosition(double position) noexcept {
  if (position != position_) {
    const double old_position = position_;
    position_ = position;
    performer_.SetTaskPosition(this, old_position);
  }
}

void BarelyTask::SetPriority(int priority) noexcept {
  if (priority != priority_) {
    const int old_priority = priority_;
    priority_ = priority;
    performer_.SetTaskPriority(this, old_priority);
  }
}
