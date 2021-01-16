#include "barelymusician/engine/clock.h"

namespace barelyapi {

Clock::Clock() : position_(0.0), tempo_(0.0) {}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

void Clock::SetPosition(double position) { position_ = position; }

void Clock::SetTempo(double tempo) { tempo_ = tempo; }

void Clock::UpdatePosition(double seconds) {
  position_ += tempo_ * seconds / kSecondsFromMinutes;
}

}  // namespace barelyapi
