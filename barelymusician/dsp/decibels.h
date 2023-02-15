#ifndef BARELYMUSICIAN_DSP_DECIBELS_H_
#define BARELYMUSICIAN_DSP_DECIBELS_H_

namespace barely {

/// Minimum decibel threshold.
inline constexpr double kMinDecibels = -80.0;

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in decibels.
/// @return Value in linear amplitude.
double AmplitudeFromDecibels(double decibels) noexcept;

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in decibels.
double DecibelsFromAmplitude(double amplitude) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DECIBELS_H_
