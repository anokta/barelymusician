#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <cassert>
#include <cmath>
#include <numbers>

#include "barelymusician.h"
#include "private/random_impl.h"

namespace barely {

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator {
 public:
  /// Generates the next output sample.
  ///
  /// @tparam kShape Oscillator shape.
  /// @param pulse_width Pulse width.
  /// @return Next output sample.
  template <OscShape kShape>
  [[nodiscard]] float Next([[maybe_unused]] float pulse_width) noexcept {
    float output = 0.0f;
    if constexpr (kShape == OscShape::kSine) {
      output = std::sin(phase_ * 2.0f * std::numbers::pi_v<float>);
    } else if constexpr (kShape == OscShape::kSaw) {
      output = 2.0f * phase_ - 1.0f;
    } else if constexpr (kShape == OscShape::kSquare) {
      output = (phase_ < pulse_width) ? -1.0f : 1.0f;
    } else if constexpr (kShape == OscShape::kNoise) {
      output = random_.DrawUniform(-1.0f, 1.0f);
    }
    phase_ += increment_;
    if (phase_ >= 1.0f) {
      phase_ -= 1.0f;
    }
    return output;
  }

  /// Resets the phase.
  void Reset() noexcept { phase_ = 0.0f; }

  /// Sets the increment per sample.
  ///
  /// @param pitch Note pitch.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param sample_interval Sample interval in seconds.
  void SetIncrement(float pitch, float reference_frequency, float sample_interval) noexcept {
    assert(reference_frequency >= 0.0f);
    assert(sample_interval >= 0.0f);
    increment_ = std::pow(2.0f, pitch) * reference_frequency * sample_interval;
  }

 private:
  // Increment per sample.
  float increment_ = 0.0f;

  // Internal clock.
  float phase_ = 0.0f;

  // White noise random number generator.
  inline static RandomImpl random_ = RandomImpl();
};

/// Normalizes the pulse width into audible range.
///
/// @param pulse_width Pulse width.
inline float NormalizePulseWidth(float pulse_width) noexcept { return 0.01f + pulse_width * 0.98f; }

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
