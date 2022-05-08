#include "barelymusician/engine/clock.h"

#include <cassert>

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

double Clock::GetBeats(double seconds) const noexcept {
  return tempo_ * seconds * kMinutesFromSeconds;
}

double Clock::GetSeconds(double beats) const noexcept {
  assert(tempo_ > 0.0);
  return beats * kSecondsFromMinutes / tempo_;
}

double Clock::GetTempo() const noexcept { return tempo_; }

void Clock::SetTempo(double tempo) noexcept {
  assert(tempo >= 0.0);
  tempo_ = tempo;
}

}  // namespace barely::internal
