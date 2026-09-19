#ifndef BARELYMUSICIAN_DSP_DISTORTION_H_
#define BARELYMUSICIAN_DSP_DISTORTION_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Applies soft-clip to the input sample.
[[nodiscard]] inline float SoftClip(float input, float drive) noexcept {
  return std::tanh(input * drive);
}

// Applies soft-clip distortion effect to the input sample.
[[nodiscard]] inline float Distortion(float input, float mix, float drive) noexcept {
  assert(mix >= 0.0f);
  assert(drive >= 0.0f);
  return std::lerp(input, SoftClip(input, drive), mix);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DISTORTION_H_
