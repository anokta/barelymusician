#ifndef BARELYMUSICIAN_COMMON_TIME_H_
#define BARELYMUSICIAN_COMMON_TIME_H_

#include <cassert>
#include <cmath>
#include <cstdint>

namespace barely {

/// Converts minutes to seconds.
inline constexpr double kMinutesToSeconds = 60.0;

/// Converts seconds to minutes.
inline constexpr double kSecondsToMinutes = 1.0 / kMinutesToSeconds;

/// Converts beats to ticks.
inline constexpr double kBeatsToTicks = static_cast<double>(1 << 24);

/// Returns the corresponding number of seconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of seconds.
[[nodiscard]] constexpr double BeatsToSeconds(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kMinutesToSeconds / tempo;
}

/// Returns the corresponding number of ticks for a given number of beats.
///
/// @param beats Number of beats.
/// @return Number of ticks.
[[nodiscard]] constexpr int64_t BeatsToTicks(double beats) noexcept {
  return static_cast<int64_t>(beats * kBeatsToTicks + (beats >= 0.0 ? 0.5 : -0.5));
}

/// Returns the corresponding number of seconds for a given number of samples.
///
/// @param sample_rate Sample rate in hertz.
/// @param samples Number of samples.
/// @return Number of seconds.
constexpr double SamplesToSeconds(int sample_rate, int64_t samples) noexcept {
  assert(sample_rate > 0);
  return static_cast<double>(samples) / static_cast<double>(sample_rate);
}

/// Returns the corresponding number of beats for a given number of seconds.
///
/// @param tempo Tempo in beats per minute.
/// @param seconds Number of seconds.
/// @return Number of beats.
[[nodiscard]] constexpr double SecondsToBeats(double tempo, double seconds) noexcept {
  return seconds * kSecondsToMinutes * tempo;
}

/// Returns the corresponding number of samples for a given number of seconds.
///
/// @param sample_rate Sample rate in hertz.
/// @param seconds Number of seconds.
/// @return Number of samples.
[[nodiscard]] constexpr int64_t SecondsToSamples(int sample_rate, double seconds) noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(sample_rate));
}

/// Returns the corresponding number of ticks for a given number of beats.
///
/// @param ticks Number of ticks.
/// @return Number of beats.
[[nodiscard]] constexpr double TicksToBeats(int64_t ticks) noexcept {
  return static_cast<double>(ticks) / kBeatsToTicks;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_TIME_H_
