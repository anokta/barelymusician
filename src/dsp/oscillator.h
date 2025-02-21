#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator {
 public:
  /// Generates the next output sample.
  ///
  /// @param shape Oscillator shape.
  /// @param skew Oscillator skew.
  /// @return Next output sample.
  [[nodiscard]] float Next(float shape, float skew) noexcept {
    float output = 0.0f;
    const float skewed_phase = std::min(1.0f, (1.0f + skew) * phase_);
    const float scaled_shape = shape * kShapeScale;
    if (shape < kShapeSineToTriangle) {
      output = std::lerp(Sine(skewed_phase), Triangle(skewed_phase), scaled_shape);
    } else if (shape < kShapeTriangleToSquare) {
      output = std::lerp(Triangle(skewed_phase), Square(skewed_phase),
                         scaled_shape - kShapeTriangleOffset);
    } else {
      output = std::lerp(Square(skewed_phase), Sawtooth(skewed_phase),
                         scaled_shape - kShapeSquareOffset);
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
  [[nodiscard]] float Sine(float phase) noexcept {
    return std::sin(phase * 2.0f * std::numbers::pi_v<float>);
  }
  [[nodiscard]] float Triangle(float phase) noexcept {
    return 4.0f * std::abs(phase - std::floor(phase + 0.75f) + 0.25f) - 1.0f;
  }
  [[nodiscard]] float Square(float phase) noexcept { return (phase < 0.5f) ? 1.0f : -1.0f; }
  [[nodiscard]] float Sawtooth(float phase) noexcept {
    return 2.0f * (phase - std::floor(phase + 0.5f));
  }

  static inline constexpr float kShapeScale = 3.0f;
  static inline constexpr float kShapeTriangleOffset = 1.0f;
  static inline constexpr float kShapeSineToTriangle = kShapeTriangleOffset / kShapeScale;
  static inline constexpr float kShapeSquareOffset = 2.0f;
  static inline constexpr float kShapeTriangleToSquare = kShapeSquareOffset / kShapeScale;

  // Increment per sample.
  float increment_ = 0.0f;

  // Internal clock.
  float phase_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
