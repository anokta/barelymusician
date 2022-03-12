#include "barelymusician/presets/dsp/dsp_utils.h"

#include <cmath>

namespace barelyapi {

namespace {

// Middle A (A4) frequency.
constexpr double kFrequencyA4 = 440.0;

}  // namespace

double AmplitudeFromDecibels(double decibels) noexcept {
  if (decibels > kMinDecibels) {
    // A = 10 ^ (dB / 20).
    return std::pow(10.0, 0.05 * decibels);
  }
  return 0.0;
}

double DecibelsFromAmplitude(double amplitude) noexcept {
  if (amplitude > 0.0) {
    // dB = 20 * log(A).
    return 20.0 * std::log10(amplitude);
  }
  return kMinDecibels;
}

double GetFilterCoefficient(int sample_rate,
                            double cuttoff_frequency) noexcept {
  if (const double sample_rate_double = static_cast<double>(sample_rate);
      sample_rate_double > 0.0 && cuttoff_frequency < sample_rate_double) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / sample_rate_double);
  }
  return 0.0;
}

double GetFrequency(double pitch) noexcept {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ p.
  return kFrequencyA4 * std::pow(2.0, pitch);
}

}  // namespace barelyapi
