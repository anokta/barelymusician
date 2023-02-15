
#include "barelymusician/dsp/decibels.h"

#include <cmath>

namespace barely {

double AmplitudeFromDecibels(double decibels) noexcept {
  if (decibels > kMinDecibels) {
    // amplitude = 10 ^ (decibels / 20).
    return std::pow(10.0, 0.05 * decibels);
  }
  return 0.0;
}

double DecibelsFromAmplitude(double amplitude) noexcept {
  if (amplitude > 0.0) {
    // decibels = 20 * log(amplitude).
    return 20.0 * std::log10(amplitude);
  }
  return kMinDecibels;
}

}  // namespace barely
