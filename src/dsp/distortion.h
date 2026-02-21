#ifndef BARELYMUSICIAN_DSP_DISTORTION_H_
#define BARELYMUSICIAN_DSP_DISTORTION_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// [1.0f, 20.0f]
inline constexpr float kDistortionDriveRange = 19.0f;

// Applies soft-clip distortion effect to the input sample.
[[nodiscard]] inline float Distortion(float input, float mix, float drive) noexcept {
  assert(mix >= 0.0f);
  assert(drive >= 0.0f);
  return std::lerp(input, std::tanh(input * drive), mix);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
