#include "barelymusician/engine/clock.h"

#include <cassert>
#include <cmath>
#include <utility>

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

double Clock::GetDuration(double timestamp) const noexcept {
  assert(timestamp >= timestamp_);
  return tempo_ * (timestamp - timestamp_) * kMinutesFromSeconds;
}

double Clock::GetTempo() const noexcept { return tempo_; }

double Clock::GetTimestamp() const noexcept { return timestamp_; }

double Clock::GetTimestamp(double duration) const noexcept {
  assert(duration >= 0.0);
  assert(tempo_ > 0.0);
  return timestamp_ + duration * kSecondsFromMinutes / tempo_;
}

void Clock::SetTempo(double tempo) noexcept {
  assert(tempo >= 0.0);
  tempo_ = tempo;
}

void Clock::SetTimestamp(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  timestamp_ = timestamp;
}

}  // namespace barely::internal
