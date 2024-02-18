#ifndef BARELYMUSICIAN_INTERNAL_SECONDS_H_
#define BARELYMUSICIAN_INTERNAL_SECONDS_H_

#include <cstdint>

namespace barely {

/// Returns the corresponding number of beats for a given number of seconds.
///
/// @param tempo Tempo in beats per minute.
/// @param seconds Number of seconds.
/// @return Number of beats.
double BeatsFromSeconds(double tempo, double seconds) noexcept;

/// Returns the corresponding number of frames for a given number of seconds.
///
/// @param frame_rate Frame rate in hertz.
/// @param seconds Number of seconds.
/// @return Number of frames.
int64_t FramesFromSeconds(int frame_rate, double seconds) noexcept;

/// Returns the corresponding number of seconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of seconds.
double SecondsFromBeats(double tempo, double beats) noexcept;

/// Returns the corresponding number of seconds for a given number of frames.
///
/// @param frame_rate Frame rate in hertz.
/// @param frames Number of frames.
/// @return Number of seconds.
double SecondsFromFrames(int frame_rate, int64_t frames) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_SECONDS_H_
