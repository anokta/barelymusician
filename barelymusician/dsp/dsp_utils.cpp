#include "barelymusician/dsp/dsp_utils.h"

#include <cassert>
#include <cmath>

namespace barely {

namespace {

// Middle A (A4) frequency.
constexpr double kFrequencyA4 = 440.0;

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

double AmplitudeFromDecibels(double decibels) noexcept {
  if (decibels > kMinDecibels) {
    // A = 10 ^ (dB / 20).
    return std::pow(10.0, 0.05 * decibels);
  }
  return 0.0;
}

double BeatsFromSeconds(double tempo, double seconds) {
  assert(tempo > 0.0);
  return tempo * seconds * kMinutesFromSeconds;
}

double DecibelsFromAmplitude(double amplitude) noexcept {
  if (amplitude > 0.0) {
    // dB = 20 * log(A).
    return 20.0 * std::log10(amplitude);
  }
  return kMinDecibels;
}

Integer FramesFromSeconds(Integer frame_rate, double seconds) {
  assert(frame_rate > 0);
  return static_cast<Integer>(seconds * static_cast<double>(frame_rate));
}

double GetFilterCoefficient(Integer frame_rate,
                            double cuttoff_frequency) noexcept {
  if (const double frame_rate_double = static_cast<double>(frame_rate);
      frame_rate_double > 0.0 && cuttoff_frequency < frame_rate_double) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / frame_rate_double);
  }
  return 0.0;
}

double GetFrequency(double pitch) noexcept {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ p.
  return kFrequencyA4 * std::pow(2.0, pitch);
}

double SecondsFromBeats(double tempo, double beats) {
  assert(tempo > 0.0);
  return beats * kSecondsFromMinutes / tempo;
}

double SecondsFromFrames(Integer frame_rate, Integer frames) {
  assert(frame_rate > 0);
  return static_cast<double>(frames) / static_cast<double>(frame_rate);
}

}  // namespace barely
