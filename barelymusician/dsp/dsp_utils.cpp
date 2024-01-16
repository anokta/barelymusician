#include "barelymusician/dsp/dsp_utils.h"

#include <cmath>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Middle A (A4) frequency.
constexpr double kFrequencyA4 = 440.0;

}  // namespace

double GetFilterCoefficient(int frame_rate, double cuttoff_frequency) noexcept {
  if (const double frame_rate_double = static_cast<double>(frame_rate);
      frame_rate_double > 0.0 && cuttoff_frequency < frame_rate_double) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / frame_rate_double);
  }
  return 0.0;
}

double GetFrequency(Rational pitch) noexcept {
  // Middle A note (A4) is selected as the base note frequency, where f = fA4 * 2 ^ p.
  return kFrequencyA4 * std::pow(2.0, static_cast<double>(pitch));
}

}  // namespace barely
