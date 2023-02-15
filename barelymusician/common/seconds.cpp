
#include "barelymusician/common/seconds.h"

#include <cassert>

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

int FramesFromSeconds(int frame_rate, double seconds) noexcept {
  assert(frame_rate > 0);
  return static_cast<int>(seconds * static_cast<double>(frame_rate));
}

double SecondsFromBeats(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kSecondsFromMinutes / tempo;
}

double SecondsFromFrames(int frame_rate, int frames) noexcept {
  assert(frame_rate > 0);
  return static_cast<double>(frames) / static_cast<double>(frame_rate);
}

}  // namespace barely
