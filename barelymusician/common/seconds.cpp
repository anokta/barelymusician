
#include "barelymusician/common/seconds.h"

#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"

namespace barely {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

double BeatsFromSeconds(double tempo, double seconds) noexcept {
  assert(tempo > 0.0);
  return tempo * seconds * kMinutesFromSeconds;
}

int FramesFromSeconds(int frame_rate, Rational seconds) noexcept {
  assert(frame_rate > 0);
  return static_cast<int>(seconds * frame_rate);
}

double SecondsFromBeats(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kSecondsFromMinutes / tempo;
}

Rational SecondsFromFrames(int frame_rate, int frames) noexcept {
  assert(frame_rate > 0);
  return Rational(frames, frame_rate);
}

}  // namespace barely
