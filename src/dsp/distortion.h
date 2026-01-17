#ifndef BARELYMUSICIAN_DSP_DISTORTION_H_
#define BARELYMUSICIAN_DSP_DISTORTION_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Applies soft-clip distortion effect to the input sample.
[[nodiscard]] inline float Distortion(float input, float amount, float drive) noexcept {
  assert(amount >= 0.0f);
  assert(drive >= 0.0f);
  return std::lerp(input, std::tanh(input * drive), amount);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
