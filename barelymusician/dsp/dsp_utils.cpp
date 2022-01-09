#include "barelymusician/dsp/dsp_utils.h"

#include <cmath>

namespace barelyapi {

namespace {

// Middle A (A4) frequency.
constexpr float kFrequencyA4 = 440.0f;

}  // namespace

float AmplitudeFromDecibels(float decibels) noexcept {
  if (decibels > kMinDecibels) {
    // A = 10 ^ (dB / 20).
    return std::pow(10.0f, 0.05f * decibels);
  }
  return 0.0f;
}

float DecibelsFromAmplitude(float amplitude) noexcept {
  if (amplitude > 0.0f) {
    // dB = 20 * log(A).
    return 20.0f * std::log10(amplitude);
  }
  return kMinDecibels;
}

float GetFilterCoefficient(int sample_rate, float cuttoff_frequency) noexcept {
  if (const float sample_rate_float = static_cast<float>(sample_rate);
      sample_rate_float > 0.0f && cuttoff_frequency < sample_rate_float) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / sample_rate_float);
  }
  return 0.0f;
}

float GetFrequency(float pitch) noexcept {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ p.
  return kFrequencyA4 * std::pow(2.0f, pitch);
}

int SamplesFromSeconds(int sample_rate, double seconds) noexcept {
  return sample_rate > 0
             ? static_cast<int>(seconds * static_cast<double>(sample_rate))
             : 0;
}

double SecondsFromSamples(int sample_rate, int samples) noexcept {
  return sample_rate > 0
             ? static_cast<double>(samples) / static_cast<double>(sample_rate)
             : 0.0;
}

}  // namespace barelyapi
