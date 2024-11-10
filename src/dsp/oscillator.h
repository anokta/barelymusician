#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <array>
#include <cmath>
#include <numbers>

#include "barelymusician.h"
#include "common/random.h"

namespace barely::internal {

/// Oscillator callback signature alias.
///
/// @param increment Increment amount.
/// @param phase Mutable phase in range [0, 1).
/// @return Output sample.
using OscillatorCallback = double (*)(double increment, double& phase);

/// Generates the next oscillator output.
///
/// @tparam kShape Oscillator shape.
/// @param increment Increment amount.
/// @param phase Mutable phase in range [0, 1).
/// @return Next output sample.
template <OscillatorShape kShape>
double Oscillator(double increment, double& phase) noexcept {
  double output = 0.0;
  // Generate the next sample.
  if constexpr (kShape == OscillatorShape::kSine) {
    output = std::sin(phase * 2.0 * std::numbers::pi_v<double>);
  } else if constexpr (kShape == OscillatorShape::kSaw) {
    output = 2.0 * phase - 1.0;
  } else if constexpr (kShape == OscillatorShape::kSquare) {
    output = (phase < 0.5) ? -1.0 : 1.0;
  } else if constexpr (kShape == OscillatorShape::kNoise) {
    static Random random;
    output = random.DrawUniform(-1.0, 1.0);
  }
  // Update the phasor.
  phase += increment;
  if (phase >= 1.0) {
    phase -= 1.0;
  }
  return output;
}

/// Array of oscillator callbacks for each shape.
inline constexpr std::array<OscillatorCallback, static_cast<int>(BarelyOscillatorShape_kCount)>
    kOscillatorCallbacks = {
        &Oscillator<OscillatorShape::kNone>,  &Oscillator<OscillatorShape::kSine>,
        &Oscillator<OscillatorShape::kSaw>,   &Oscillator<OscillatorShape::kSquare>,
        &Oscillator<OscillatorShape::kNoise>,
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
