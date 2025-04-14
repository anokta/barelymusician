#include "api/trigger.h"

#include <barelymusician.h>

#include <cassert>

#include "api/performer.h"

BarelyTrigger::BarelyTrigger(BarelyPerformer& performer, double position,
                             ProcessCallback callback) noexcept
    : performer_(performer), position_(position), process_callback_(callback) {
  performer_.AddTrigger(this);
}

BarelyTrigger::~BarelyTrigger() noexcept { performer_.RemoveTrigger(this); }

void BarelyTrigger::SetPosition(double position) noexcept {
  if (position != position_) {
    const double old_position = position_;
    position_ = position;
    performer_.SetTriggerPosition(this, old_position);
  }
}
