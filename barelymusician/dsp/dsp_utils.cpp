#include "barelymusician/dsp/dsp_utils.h"

#include <cmath>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Middle A (A4) frequency.
constexpr float kFrequencyA4 = 440.0f;

}  // namespace

float GetFilterCoefficient(int frame_rate, float cuttoff_frequency) noexcept {
  if (const float frame_rate_float = static_cast<float>(frame_rate);
      frame_rate_float > 0.0f && cuttoff_frequency < frame_rate_float) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / frame_rate_float);
  }
  return 0.0;
}

float GetFrequency(Rational pitch) noexcept {
  // Middle A note (A4) is selected as the base note frequency, where f = fA4 * 2 ^ p.
  return kFrequencyA4 * std::pow(2.0f, static_cast<float>(pitch));
}

}  // namespace barely
