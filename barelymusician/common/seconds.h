#ifndef BARELYMUSICIAN_COMMON_SECONDS_H_
#define BARELYMUSICIAN_COMMON_SECONDS_H_

#include "barelymusician/barelymusician.h"

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
int FramesFromSeconds(int frame_rate, Rational seconds) noexcept;

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
Rational SecondsFromFrames(int frame_rate, int frames) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_SECONDS_H_
