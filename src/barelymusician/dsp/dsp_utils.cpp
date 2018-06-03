#include "barelymusician/dsp/dsp_utils.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

float AmplitudeFromDecibels(float decibels) {
  if (decibels > kMinDecibels) {
    // A = 10 ^ (dB / 20).
    return std::pow(10.0f, 0.05f * decibels);
  }
  return 0.0f;
}

float DecibelsFromAmplitude(float amplitude) {
  if (amplitude > 0.0f) {
    // dB = 20 * log(A).
    return 20.0f * std::log10(amplitude);
  }
  return kMinDecibels;
}

float GetFilterCoefficient(int sample_rate, float cuttoff_frequency) {
  DCHECK_GT(sample_rate, 0);
  const float sample_rate_float = static_cast<float>(sample_rate);
  if (cuttoff_frequency < sample_rate_float) {
    // c = exp(-2 * pi * fc / fs).
    // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
    return std::exp(-kTwoPi * cuttoff_frequency / sample_rate_float);
  }
  return 0.0f;
}

}  // namespace barelyapi
