#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <cassert>
#include <cmath>
#include <numbers>

#include "barelymusician.h"
#include "common/random.h"

namespace barely::internal {

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator {
 public:
  /// Generates the next output sample.
  ///
  /// @tparam kShape Oscillator shape.
  /// @return Next output sample.
  template <OscillatorShape kShape>
  [[nodiscard]] double Next() noexcept {
    double output = 0.0;
    if constexpr (kShape == OscillatorShape::kSine) {
      output = std::sin(phase_ * 2.0 * std::numbers::pi_v<double>);
    } else if constexpr (kShape == OscillatorShape::kSaw) {
      output = 2.0 * phase_ - 1.0;
    } else if constexpr (kShape == OscillatorShape::kSquare) {
      output = (phase_ < 0.5) ? -1.0 : 1.0;
    } else if constexpr (kShape == OscillatorShape::kNoise) {
      output = random_.DrawUniform(-1.0, 1.0);
    }
    phase_ += increment_;
    if (phase_ >= 1.0) {
      phase_ -= 1.0;
    }
    return output;
  }

  /// Resets the phase.
  void Reset() noexcept { phase_ = 0.0; }

  /// Sets the increment per sample.
  ///
  /// @param increment Increment per sample.
  void SetIncrement(double increment) noexcept {
    assert(increment >= 0.0);
    increment_ = increment;
  }

 private:
  // Increment per sample.
  double increment_ = 0.0;

  // Internal clock.
  double phase_ = 0.0;

  // White noise random number generator.
  inline static Random random_ = Random();
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
