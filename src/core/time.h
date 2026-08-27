#ifndef BARELYMUSICIAN_CORE_TIME_H_
#define BARELYMUSICIAN_CORE_TIME_H_

#include <cassert>
#include <cmath>
#include <cstdint>

namespace barely {

[[nodiscard]] constexpr double FramesToSeconds(float sample_rate, int64_t frames) noexcept {
  assert(sample_rate > 0);
  return static_cast<double>(frames) / static_cast<double>(sample_rate);
}

[[nodiscard]] constexpr int64_t SecondsToFrames(float sample_rate, double seconds) noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(sample_rate));
}

[[nodiscard]] constexpr double Quantize(double position, int32_t subdivision,
                                        float amount) noexcept {
  assert(subdivision > 0);
  assert(amount >= 0.0f && amount <= 1.0f);
  return std::lerp(
      position,
      static_cast<double>(static_cast<int64_t>(static_cast<double>(subdivision) * position +
                                               (position >= 0.0 ? 0.5 : -0.5))) /
          static_cast<double>(subdivision),
      static_cast<double>(amount));
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_TIME_H_
