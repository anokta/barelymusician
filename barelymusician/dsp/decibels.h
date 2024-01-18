#ifndef BARELYMUSICIAN_DSP_DECIBELS_H_
#define BARELYMUSICIAN_DSP_DECIBELS_H_

namespace barely {

/// Minimum decibel threshold.
inline constexpr float kMinDecibels = -80.0f;

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in decibels.
/// @return Value in linear amplitude.
float AmplitudeFromDecibels(float decibels) noexcept;

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in decibels.
float DecibelsFromAmplitude(float amplitude) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DECIBELS_H_
