#ifndef EXAMPLES_DSP_OSCILLATOR_H_
#define EXAMPLES_DSP_OSCILLATOR_H_

#include <random>

#include "examples/dsp/generator.h"

namespace barelyapi {

/// Oscillator type.
enum class OscillatorType {
  /// Sine wave.
  kSine = 0,
  /// Sawtooth wave.
  kSaw = 1,
  /// Square wave.
  kSquare = 2,
  /// White noise.
  kNoise = 3,
};

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator : public Generator {
 public:
  /// Constructs new `Oscillator`.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit Oscillator(int sample_rate) noexcept;

  /// Implements `Generator`.
  float Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the frequency of the oscillator.
  ///
  /// @param frequency Oscillator frequency in hz.
  void SetFrequency(float frequency) noexcept;

  /// Sets the type of the oscillator.
  ///
  /// @param type Oscillator type.
  void SetType(OscillatorType type) noexcept;

 private:
  // Inverse sampling rate in seconds.
  float sample_interval_;

  // Oscillator type.
  OscillatorType type_;

  // Increment per sample.
  float increment_;

  // Internal clock.
  float phase_;

  // White noise random number generator.
  std::uniform_real_distribution<float> white_noise_distribution_;
  std::default_random_engine white_noise_engine_;
};

}  // namespace barelyapi

#endif  // EXAMPLES_DSP_OSCILLATOR_H_
