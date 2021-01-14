#include "barelymusician/engine/clock.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Clock::Clock() : position_(0.0), tempo_(0.0) {}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

void Clock::SetPosition(double position) {
  DCHECK_GE(position, 0.0);
  position_ = position;
}

void Clock::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  tempo_ = tempo;
}

void Clock::UpdatePosition(double seconds) {
  DCHECK_GE(seconds, 0.0);
  position_ += tempo_ * seconds / kSecondsFromMinutes;
}

}  // namespace barelyapi
