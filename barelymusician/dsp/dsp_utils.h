#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

#include "barelymusician/barelymusician.h"

namespace barely {

/// Pi.
inline constexpr float kPi = 3.14159265358979323846f;

/// 2 * pi.
inline constexpr float kTwoPi = 2.0f * kPi;

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param frame_rate Frame rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
float GetFilterCoefficient(int frame_rate, float cuttoff_frequency) noexcept;

/// Returns frequency for the given pitch.
///
/// @param pitch Pitch.
/// @return Frequency in hertz.
float GetFrequency(Rational pitch) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
