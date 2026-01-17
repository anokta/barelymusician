#ifndef BARELYMUSICIAN_DSP_DECIBELS_H_
#define BARELYMUSICIAN_DSP_DECIBELS_H_

#include <cmath>

namespace barely {

// Minimum decibel threshold.
inline constexpr float kMinDecibels = -80.0f;

inline constexpr float AmplitudeToDecibels(float amplitude) noexcept {
  // decibels = 20 * log(amplitude).
  return (amplitude > 0.0) ? 20.0f * std::log10(amplitude) : kMinDecibels;
}

inline constexpr float DecibelsToAmplitude(float decibels) noexcept {
  // amplitude = 10 ^ (decibels / 20).
  return (decibels > kMinDecibels) ? std::pow(10.0f, 0.05f * decibels) : 0.0f;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DECIBELS_H_
