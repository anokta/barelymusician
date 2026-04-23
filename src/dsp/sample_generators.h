#ifndef BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
#define BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace barely {

inline constexpr float kOscSkewRange = 0.25f;

[[nodiscard]] inline float PolyBlep(float phase, float increment) noexcept {
  if (phase < increment) {
    const float t = phase / increment;
    return t + t - t * t - 1.0f;
  }
  if (phase > 1.0f - increment) {
    const float t = (phase - 1.0f) / increment;
    return t * t + t + t + 1.0f;
  }
  return 0.0f;
}

[[nodiscard]] inline float GenerateSineSample(float phase) noexcept {
  return std::sin(phase * 2.0f * std::numbers::pi_v<float>);
}

[[nodiscard]] inline float GenerateTriangleSample(float phase) noexcept {
  return 4.0f * std::abs(phase - static_cast<float>(phase + 0.75f >= 1.0f) + 0.25f) - 1.0f;
}

[[nodiscard]] inline float GenerateSquareSample(float phase, float increment) noexcept {
  const float shifted_phase = phase + 0.5f;
  return ((phase < 0.5f) ? 1.0f : -1.0f) + PolyBlep(phase, increment) -
         PolyBlep(shifted_phase - static_cast<float>(shifted_phase >= 1.0f), increment);
}

[[nodiscard]] inline float GenerateSawtoothSample(float phase, float increment) noexcept {
  const float shifted_phase = phase + 0.5f;
  const float shifted_phase_floor = static_cast<float>(shifted_phase >= 1.0f);
  return 2.0f * (phase - shifted_phase_floor) -
         PolyBlep(shifted_phase - shifted_phase_floor, increment);
}

[[nodiscard]] inline float GenerateOscSample(float osc_shape, float osc_phase,
                                             float osc_increment) noexcept {
  assert(osc_shape >= 0.0f && osc_shape <= 1.0f && "GenerateOscSample");
  assert(osc_phase >= 0.0f && osc_phase <= 1.0f && "GenerateOscSample");
  assert(osc_increment > 0.0f && osc_increment <= 0.5f && "GenerateOscSample");
  static constexpr float kShapeScale = 3.0f;
  static constexpr float kShapeTriangleOffset = 1.0f;
  static constexpr float kShapeSineToTriangle = kShapeTriangleOffset / kShapeScale;
  static constexpr float kShapeSquareOffset = 2.0f;
  static constexpr float kShapeTriangleToSquare = kShapeSquareOffset / kShapeScale;
  const float scaled_shape = osc_shape * kShapeScale;
  if (osc_shape < kShapeSineToTriangle) {
    return std::lerp(GenerateSineSample(osc_phase), GenerateTriangleSample(osc_phase),
                     scaled_shape);
  }
  if (osc_shape < kShapeTriangleToSquare) {
    return std::lerp(GenerateTriangleSample(osc_phase),
                     GenerateSquareSample(osc_phase, osc_increment),
                     scaled_shape - kShapeTriangleOffset);
  }
  return std::lerp(GenerateSquareSample(osc_phase, osc_increment),
                   GenerateSawtoothSample(osc_phase, osc_increment),
                   scaled_shape - kShapeSquareOffset);
}

[[nodiscard]] inline float GenerateSliceSample(const float* samples, int32_t sample_count,
                                               float offset) noexcept {
  assert((samples != nullptr || sample_count == 0) && "GenerateSliceSample");
  assert(offset >= 0.0f && "GenerateSliceSample");
  const int32_t index = static_cast<int32_t>(offset);
  return (index < sample_count) ? std::lerp(samples[index], samples[(index + 1) % sample_count],
                                            offset - static_cast<float>(index))
                                : 0.0f;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
