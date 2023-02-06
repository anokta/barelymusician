#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

#include "barelymusician/barelymusician.h"

namespace barely {

/// PI.
inline constexpr Real kPi = 3.14159265358979323846;
inline constexpr Real kTwoPi = 2.0 * kPi;

/// Minimum decibel threshold.
inline constexpr Real kMinDecibels = -80.0;

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in dB.
/// @return Value in linear amplitude.
Real AmplitudeFromDecibels(Real decibels) noexcept;

/// Returns the corresponding number of beats for a given number of nanoseconds.
///
/// @param tempo Tempo in beats per minute.
/// @param nanoseconds Number of nanoseconds.
/// @return Number of beats.
Real BeatsFromNanoseconds(Real tempo, Integer nanoseconds);

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in dB.
Real DecibelsFromAmplitude(Real amplitude) noexcept;

/// Returns the corresponding number of frames for a given number of
/// nanoseconds.
///
/// @param frame_rate Frame rate in hertz.
/// @param nanoseconds Number of nanoseconds.
/// @return Number of frames.
Integer FramesFromNanoseconds(Integer frame_rate, Integer nanoseconds);

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param frame_rate Frame rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
Real GetFilterCoefficient(Integer frame_rate, Real cuttoff_frequency) noexcept;

/// Returns frequency for the given pitch.
///
/// @param pitch Pitch.
/// @return Frequency in hertz.
Real GetFrequency(Real pitch) noexcept;

/// Returns the corresponding number of nanoseconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of nanoseconds.
Integer NanosecondsFromBeats(Real tempo, Real beats);

/// Returns the corresponding number of nanoseconds for a given number of
/// frames.
///
/// @param frame_rate Frame rate in hertz.
/// @param frames Number of frames.
/// @return Number of nanoseconds.
Integer NanosecondsFromFrames(Integer frame_rate, Integer frames);

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
