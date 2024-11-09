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
/// @param phase Phase in range [0, 1).
using OscillatorCallback = double (*)(double phase);

/// Array of oscillator callbacks for each shape.
inline constexpr std::array<OscillatorCallback, static_cast<int>(BarelyOscillatorShape_kCount)>
    kOscillatorCallbacks = {
        // BarelyOscillatorShape_kNone
        [](double /*phase*/) { return 0.0; },
        // BarelyOscillatorShape_kSine
        [](double phase) { return std::sin(phase * 2.0 * std::numbers::pi_v<double>); },
        // BarelyOscillatorShape_kSaw
        [](double phase) { return 2.0 * phase - 1.0; },
        // BarelyOscillatorShape_kSquare
        [](double phase) { return (phase < 0.5) ? -1.0 : 1.0; },
        // BarelyOscillatorShape_kRandom
        [](double /*phase*/) {
          static Random random;
          return random.DrawUniform(-1.0, 1.0);
        }};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
