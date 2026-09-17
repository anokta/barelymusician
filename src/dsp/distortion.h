#ifndef BARELYMUSICIAN_DSP_DISTORTION_H_
#define BARELYMUSICIAN_DSP_DISTORTION_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Applies soft-clip to the input sample.
template <typename T>
[[nodiscard]] T SoftClip(T input, T drive) noexcept {
  return std::tanh(input * drive);
}

// Applies soft-clip distortion effect to the input sample.
[[nodiscard]] inline double Distortion(double input, double mix, double drive) noexcept {
  assert(mix >= 0.0);
  assert(drive >= 0.0);
  return std::lerp(input, SoftClip(input, drive), mix);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DISTORTION_H_
