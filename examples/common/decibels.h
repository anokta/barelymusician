#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_DECIBELS_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_DECIBELS_H_

#include <cmath>

namespace barely::examples {

/// Minimum decibel threshold.
inline constexpr float kMinDecibels = -80.0f;

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in decibels.
inline constexpr float AmplitudeToDecibels(float amplitude) noexcept {
  return (amplitude > 0.0) ? 20.0f * std::log10(amplitude) : kMinDecibels;
}

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in decibels.
/// @return Value in linear amplitude.
inline constexpr float DecibelsToAmplitude(float decibels) noexcept {
  return (decibels > kMinDecibels) ? std::pow(10.0f, 0.05f * decibels) : 0.0f;
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_DECIBELS_H_
