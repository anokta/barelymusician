#ifndef BARELYMUSICIAN_DSP_DISTORTION_H_
#define BARELYMUSICIAN_DSP_DISTORTION_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Applies soft-clip distortion effect to the input sample.
[[nodiscard]] inline float Distortion(float input, float mix, float drive) noexcept {
  assert(mix >= 0.0f);
  assert(drive >= 0.0f);
  return std::lerp(input, std::tanh(input * drive), mix);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DISTORTION_H_
