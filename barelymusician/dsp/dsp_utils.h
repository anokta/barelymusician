#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

#include <numbers>

#include "barelymusician/barelymusician.h"

namespace barely {

/// 2 * pi.
inline constexpr float kTwoPi = 2.0f * std::numbers::pi_v<float>;

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
