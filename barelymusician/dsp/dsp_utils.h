#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

namespace barelyapi {

/// PI.
inline constexpr double kPi = 3.14159265358979323846;
inline constexpr double kTwoPi = 2.0 * kPi;

/// Minimum dB threshold.
inline constexpr double kMinDecibels = -80.0;

/// Converts a value from dB to linear amplitude.
///
/// @param decibels Value in dB.
/// @return Value in linear amplitude.
double AmplitudeFromDecibels(double decibels) noexcept;

/// Converts a value from linear amplitude to dB.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in dB.
double DecibelsFromAmplitude(double amplitude) noexcept;

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in hz.
/// @param cuttoff_frequency Cutoff frequency in hz.
/// @return Filter coefficient.
double GetFilterCoefficient(int sample_rate, double cuttoff_frequency) noexcept;

/// Returns frequency for the given pitch.
///
/// @param pitch Pitch.
/// @return Frequency in hz.
double GetFrequency(double pitch) noexcept;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
