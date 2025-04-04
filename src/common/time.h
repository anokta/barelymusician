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

/// Returns the corresponding number of seconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of seconds.
[[nodiscard]] constexpr double BeatsToSeconds(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kMinutesToSeconds / tempo;
}

/// Quantizes a position.
///
/// @param position Position in beats.
/// @param subdivision Subdivision of a beat.
/// @param amount Quantization amount.
/// @return Position in beats.
[[nodiscard]] constexpr double Quantize(double position, int subdivision, float amount) noexcept {
  assert(subdivision > 0);
  assert(amount >= 0.0f && amount <= 0.0f);
  return std::lerp(
      position,
      static_cast<double>(static_cast<int64_t>(static_cast<double>(subdivision) * position +
                                               (position >= 0.0 ? 0.5 : -0.5))) /
          static_cast<double>(subdivision),
      static_cast<double>(amount));
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

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_TIME_H_
