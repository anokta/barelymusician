#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

namespace barelyapi {

// Converts a value from dB to linear amplitude.
//
// @param decibels Value in dB.
// @return Value in linear amplitude.
float AmplitudeFromDecibels(float decibels);

// Converts a value from linear amplitude to dB.
//
// @param amplitude Value in linear amplitude.
// @return Value in dB.
float DecibelsFromAmplitude(float amplitude);

// Returns the corresponding frequency for the given note index.
//
// @param index Note index.
// @return Frequency in Hz.
float FrequencyFromNoteIndex(float index);

// Returns one-pole filter coefficient for a given cutoff frequency.
//
// @param sample_rate Sampling rate in Hz.
// @param cuttoff_frequency Cutoff frequency in Hz.
// @return Filter coefficient.
float GetFilterCoefficient(int sample_rate, float cuttoff_frequency);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
