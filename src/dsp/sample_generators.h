#ifndef BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
#define BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

/// Generates a sine wave sample.
///
/// @param phase Phase.
/// @return Sample.
[[nodiscard]] inline float GenerateSineSample(float phase) noexcept {
  return std::sin(phase * 2.0f * std::numbers::pi_v<float>);
}

/// Generates a triangle wave sample.
///
/// @param phase Phase.
/// @return Sample.
[[nodiscard]] inline float GenerateTriangleSample(float phase) noexcept {
  return 4.0f * std::abs(phase - std::floor(phase + 0.75f) + 0.25f) - 1.0f;
}

/// Generates a square wave sample.
///
/// @param phase Phase.
/// @return Sample.
[[nodiscard]] inline float GenerateSquareSample(float phase) noexcept {
  return (phase < 0.5f) ? 1.0f : -1.0f;
}

/// Generates a sawtooth wave sample.
///
/// @param phase Phase.
/// @return Sample.
[[nodiscard]] inline float GenerateSawtoothSample(float phase) noexcept {
  return 2.0f * (phase - std::floor(phase + 0.5f));
}

/// Generates an oscillator sample.
///
/// @param osc_phase Oscillator phase.
/// @param osc_shape Oscillator shape.
/// @return Sample.
[[nodiscard]] inline float GenerateOscSample(float osc_phase, float osc_shape) noexcept {
  assert(osc_phase >= 0.0f && osc_phase <= 1.0f && "GenerateOscSample");
  assert(osc_shape >= 0.0f && osc_shape <= 1.0f && "GenerateOscSample");
  static constexpr float kShapeScale = 3.0f;
  static constexpr float kShapeTriangleOffset = 1.0f;
  static constexpr float kShapeSineToTriangle = kShapeTriangleOffset / kShapeScale;
  static constexpr float kShapeSquareOffset = 2.0f;
  static constexpr float kShapeTriangleToSquare = kShapeSquareOffset / kShapeScale;
  const float scaled_shape = osc_shape * kShapeScale;
  if (osc_shape < kShapeSineToTriangle) {
    return std::lerp(GenerateSineSample(osc_phase), GenerateTriangleSample(osc_phase),
                     scaled_shape);
  } else if (osc_shape < kShapeTriangleToSquare) {
    return std::lerp(GenerateTriangleSample(osc_phase), GenerateSquareSample(osc_phase),
                     scaled_shape - kShapeTriangleOffset);
  } else {
    return std::lerp(GenerateSquareSample(osc_phase), GenerateSawtoothSample(osc_phase),
                     scaled_shape - kShapeSquareOffset);
  }
}

/// Generates a slice sample.
//
/// @param slice Pointer to slice.
/// @param slice_offset Slice offset.
/// @return Sample.
[[nodiscard]] inline float GenerateSliceSample(const Slice& slice, float slice_offset) noexcept {
  const int index = static_cast<int>(slice_offset);
  return (index < slice.sample_count)
             ? std::lerp(slice.samples[index], slice.samples[(index + 1) % slice.sample_count],
                         slice_offset - static_cast<float>(index))
             : 0.0f;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
