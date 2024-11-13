#ifndef BARELYMUSICIAN_DSP_DECIBELS_H_
#define BARELYMUSICIAN_DSP_DECIBELS_H_

#include <cmath>

namespace barely::internal {

/// Minimum decibel threshold.
inline constexpr double kMinDecibels = -80.0;

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in decibels.
/// @return Value in linear amplitude.
inline double AmplitudeFromDecibels(double decibels) noexcept {
  if (decibels > kMinDecibels) {
    // amplitude = 10 ^ (decibels / 20).
    return std::pow(10.0, 0.05 * decibels);
  }
  return 0.0;
}

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in decibels.
inline double DecibelsFromAmplitude(double amplitude) noexcept {
  if (amplitude > 0.0) {
    // decibels = 20 * log(amplitude).
    return 20.0 * std::log10(amplitude);
  }
  return kMinDecibels;
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_DECIBELS_H_
