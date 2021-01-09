#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

#include <cstdint>

namespace barelyapi {

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

/// Returns the corresponding frequency for the given pitch.
///
/// @param pitch Pitch.
/// @return Frequency in Hz.
float FrequencyFromPitch(float pitch);

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in Hz.
/// @param cuttoff_frequency Cutoff frequency in Hz.
/// @return Filter coefficient.
float GetFilterCoefficient(int sample_rate, float cuttoff_frequency);

/// Returns the corresponding number of samples for the given number of seconds.
///
/// @param sample_rate Sampling rate in Hz.
/// @param seconds Number of seconds.
/// @return Number of samples.
std::int64_t SamplesFromSeconds(int sample_rate, double seconds);

/// Returns the corresponding number of seconds for the given number of samples.
///
/// @param sample_rate Sampling rate in Hz.
/// @param samples Number of samples.
/// @return Number of seconds.
double SecondsFromSamples(int sample_rate, std::int64_t samples);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
