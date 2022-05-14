#include "barelymusician/engine/clock.h"

#include <cassert>
#include <limits>

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
  return (tempo_ > 0.0)  ? beats * kSecondsFromMinutes / tempo_
         : (beats > 0.0) ? std::numeric_limits<double>::max()
         : (beats < 0.0) ? std::numeric_limits<double>::lowest()
                         : 0.0;
}

double Clock::GetTempo() const noexcept { return tempo_; }

void Clock::SetTempo(double tempo) noexcept {
  assert(tempo_ >= 0.0);
  tempo_ = tempo;
}

}  // namespace barely::internal
