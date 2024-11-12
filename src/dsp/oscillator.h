#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <cassert>
#include <cmath>
#include <numbers>

#include "barelymusician.h"
#include "common/random.h"

namespace barely::internal {

/// Generates the next oscillator output.
///
/// @tparam kShape Oscillator shape.
/// @param phase Mutable phase in range [0, 1).
/// @return Next output sample.
template <OscillatorShape kShape>
double Oscillator(double phase) noexcept {
  assert(phase >= 0.0);
  assert(phase < 1.0);
  if constexpr (kShape == OscillatorShape::kNone) {
    return 0.0;
  } else if constexpr (kShape == OscillatorShape::kSine) {
    return std::sin(phase * 2.0 * std::numbers::pi_v<double>);
  } else if constexpr (kShape == OscillatorShape::kSaw) {
    return 2.0 * phase - 1.0;
  } else if constexpr (kShape == OscillatorShape::kSquare) {
    return (phase < 0.5) ? -1.0 : 1.0;
  } else if constexpr (kShape == OscillatorShape::kNoise) {
    static Random random;
    return random.DrawUniform(-1.0, 1.0);
  }
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
