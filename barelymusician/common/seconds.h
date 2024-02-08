#ifndef BARELYMUSICIAN_COMMON_SECONDS_H_
#define BARELYMUSICIAN_COMMON_SECONDS_H_

#include <cassert>
#include <cstdint>

namespace barely {

/// Converts seconds to minutes.
inline constexpr double kMinutesFromSeconds = 1.0 / 60.0;

/// Converts minutes to seconds.
inline constexpr double kSecondsFromMinutes = 60.0;

/// Returns the corresponding number of beats for a given number of seconds.
///
/// @param tempo Tempo in beats per minute.
/// @param seconds Number of seconds.
/// @return Number of beats.
constexpr double BeatsFromSeconds(double tempo, double seconds) noexcept;

/// Returns the corresponding number of frames for a given number of seconds.
///
/// @param frame_rate Frame rate in hertz.
/// @param seconds Number of seconds.
/// @return Number of frames.
constexpr int64_t FramesFromSeconds(int frame_rate, double seconds) noexcept;

/// Returns the corresponding number of seconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of seconds.
constexpr double SecondsFromBeats(double tempo, double beats) noexcept;

/// Returns the corresponding number of seconds for a given number of frames.
///
/// @param frame_rate Frame rate in hertz.
/// @param frames Number of frames.
/// @return Number of seconds.
constexpr double SecondsFromFrames(int frame_rate, int64_t frames) noexcept;

constexpr double BeatsFromSeconds(double tempo, double seconds) noexcept {
  assert(tempo > 0.0);
  return tempo * seconds * kMinutesFromSeconds;
}

constexpr int64_t FramesFromSeconds(int frame_rate, double seconds) noexcept {
  assert(frame_rate > 0);
  return static_cast<int64_t>(seconds * static_cast<double>(frame_rate));
}

constexpr double SecondsFromBeats(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kSecondsFromMinutes / tempo;
}

constexpr double SecondsFromFrames(int frame_rate, int64_t frames) noexcept {
  assert(frame_rate > 0);
  return static_cast<double>(frames) / static_cast<double>(frame_rate);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_SECONDS_H_
