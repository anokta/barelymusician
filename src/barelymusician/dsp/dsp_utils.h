#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

namespace barelyapi {

// Returns the corresponding number of beats for the given |samples|.
//
// @param samples Number of samples.
// @param num_samples_per_beat Number of samples per beat.
// @return Fractional number of beats.
double BeatsFromSamples(int samples, int num_samples_per_beat);

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

// Returns one-pole filter coefficient for a given cutoff frequency.
//
// @param sample_rate Sampling rate in Hz.
// @param cuttoff_frequency Cutoff frequency in Hz.
// @return Filter coefficient.
float GetFilterCoefficient(int sample_rate, float cuttoff_frequency);

// Returns the corresponding number of samples for the given |beats|.
//
// @param beats Fractional number of beats.
// @param num_samples_per_beat Number of samples per beat.
// @return Number of samples.
int SamplesFromBeats(double beats, int num_samples_per_beat);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
