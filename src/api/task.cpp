#include "api/task.h"

#include <barelymusician.h>

#include <cassert>

#include "api/performer.h"

BarelyTask::BarelyTask(BarelyPerformer& performer, double position, double duration,
                       ProcessCallback callback) noexcept
    : performer_(performer), position_(position), duration_(duration), process_callback_(callback) {
  assert(duration > 0.0 && "Invalid task duration");
  performer_.CreateTask(this);
}

BarelyTask::~BarelyTask() noexcept { performer_.DestroyTask(this); }

void BarelyTask::SetDuration(double duration) noexcept {
  assert(duration > 0.0 && "Invalid task duration");
  if (duration != duration_) {
    const double old_duration = duration_;
    duration_ = duration;
    performer_.SetTaskDuration(this, old_duration);
  }
}

void BarelyTask::SetPosition(double position) noexcept {
  if (position != position_) {
    const double old_position = position_;
    position_ = position;
    performer_.SetTaskPosition(this, old_position);
  }
}

void BarelyTask::SetProcessCallback(ProcessCallback callback) noexcept {
  if (is_active_) {
    Process(BarelyTaskState_kEnd);
  }
  process_callback_ = callback;
  if (is_active_) {
    Process(BarelyTaskState_kBegin);
  }
}
