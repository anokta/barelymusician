#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

namespace barelyapi {

/// PI.
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = 2.0f * kPi;

/// Minimum dB threshold.
inline constexpr float kMinDecibels = -80.0f;

/// Converts a value from dB to linear amplitude.
///
/// @param decibels Value in dB.
/// @return Value in linear amplitude.
float AmplitudeFromDecibels(float decibels);

/// Converts a value from linear amplitude to dB.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in dB.
float DecibelsFromAmplitude(float amplitude);

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in Hz.
/// @param cuttoff_frequency Cutoff frequency in Hz.
/// @return Filter coefficient.
float GetFilterCoefficient(int sample_rate, float cuttoff_frequency);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
