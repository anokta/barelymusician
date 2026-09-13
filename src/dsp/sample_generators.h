#ifndef BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
#define BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <numbers>

#include "core/rng.h"

namespace barely {

inline constexpr double kOscSkewRange = 0.25;

[[nodiscard]] inline double PolyBlep(double phase, double increment) noexcept {
  if (phase < increment) {
    const double t = phase / increment;
    return t + t - t * t - 1.0;
  }
  if (phase > 1.0 - increment) {
    const double t = (phase - 1.0) / increment;
    return t * t + t + t + 1.0;
  }
  return 0.0;
}

[[nodiscard]] inline double GenerateSineSample(double phase) noexcept {
  return std::sin(phase * 2.0 * std::numbers::pi_v<double>);
}

[[nodiscard]] inline double GenerateTriangleSample(double phase) noexcept {
  return 4.0 * std::abs(phase - static_cast<double>(phase + 0.75 >= 1.0) + 0.25) - 1.0;
}

[[nodiscard]] inline double GenerateSquareSample(double phase, double increment) noexcept {
  const double shifted_phase = phase + 0.5;
  return ((phase < 0.5) ? 1.0 : -1.0) + PolyBlep(phase, increment) -
         PolyBlep(shifted_phase - static_cast<double>(shifted_phase >= 1.0), increment);
}

[[nodiscard]] inline double GenerateSawtoothSample(double phase, double increment) noexcept {
  const double shifted_phase = phase + 0.5;
  const double shifted_phase_floor = static_cast<double>(shifted_phase >= 1.0);
  return 2.0 * (phase - shifted_phase_floor) -
         PolyBlep(shifted_phase - shifted_phase_floor, increment);
}

[[nodiscard]] inline double GenerateOscSample(double osc_shape, double osc_phase,
                                              double osc_increment) noexcept {
  assert(osc_shape >= 0.0 && osc_shape <= 1.0 && "GenerateOscSample");
  assert(osc_phase >= 0.0 && osc_phase <= 1.0 && "GenerateOscSample");
  assert(osc_increment > 0.0 && osc_increment <= 0.5 && "GenerateOscSample");
  static constexpr double kShapeScale = 3.0;
  static constexpr double kShapeTriangleOffset = 1.0;
  static constexpr double kShapeSineToTriangle = kShapeTriangleOffset / kShapeScale;
  static constexpr double kShapeSquareOffset = 2.0;
  static constexpr double kShapeTriangleToSquare = kShapeSquareOffset / kShapeScale;
  const double scaled_shape = osc_shape * kShapeScale;
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

[[nodiscard]] inline double GenerateSliceSample(const double* samples, int32_t sample_count,
                                                double offset, bool is_looping) noexcept {
  assert((samples != nullptr || sample_count == 0) && "GenerateSliceSample");
  assert(offset >= 0.0 && "GenerateSliceSample");
  const int32_t index = static_cast<int32_t>(offset);
  return (index < sample_count)
             ? std::lerp(samples[index],
                         (index + 1 < sample_count) ? samples[index + 1]
                                                    : (is_looping ? samples[0] : 0.0),
                         offset - static_cast<double>(index))
             : 0.0;
}

template <typename RngType>
[[nodiscard]] double GenerateNoiseSample(RngType& rng) noexcept {
  return (2.0 * static_cast<double>(rng.Generate())) - 1.0;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_GENERATORS_H_
