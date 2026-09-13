#ifndef BARELYMUSICIAN_DSP_DECIBELS_H_
#define BARELYMUSICIAN_DSP_DECIBELS_H_

#include <cmath>

namespace barely {

inline constexpr double kMinDecibels = -60.0;

constexpr double AmplitudeToDecibels(double amplitude) noexcept {
  // decibels = 20 * log(amplitude).
  return (amplitude > 0.0) ? 20.0 * std::log10(amplitude) : kMinDecibels;
}

constexpr double DecibelsToAmplitude(double decibels) noexcept {
  // amplitude = 10 ^ (decibels / 20).
  return (decibels > kMinDecibels) ? std::pow(10.0, 0.05 * decibels) : 0.0;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DECIBELS_H_
