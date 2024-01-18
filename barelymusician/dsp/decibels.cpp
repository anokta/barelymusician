
#include "barelymusician/dsp/decibels.h"

#include <cmath>

namespace barely {

float AmplitudeFromDecibels(float decibels) noexcept {
  if (decibels > kMinDecibels) {
    // amplitude = 10 ^ (decibels / 20).
    return std::pow(10.0f, 0.05f * decibels);
  }
  return 0.0f;
}

float DecibelsFromAmplitude(float amplitude) noexcept {
  if (amplitude > 0.0f) {
    // decibels = 20 * log(amplitude).
    return 20.0f * std::log10(amplitude);
  }
  return kMinDecibels;
}

}  // namespace barely
