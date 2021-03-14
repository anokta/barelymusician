#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include "barelymusician/common/random_generator.h"
#include "barelymusician/dsp/generator.h"

namespace barelyapi {

// Oscillator type.
enum class OscillatorType {
  kSine,    // Sine wave.
  kSaw,     // Sawtooth wave.
  kSquare,  // Square wave.
  kNoise,   // White noise.
};

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator : public Generator {
 public:
  /// Constructs new |Oscillator|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit Oscillator(int sample_rate);

  /// Implements |Generator|.
  float Next() override;
  void Reset() override;

  /// Sets the frequency of the oscillator.
  ///
  /// @param frequency Oscillator frequency in Hz.
  void SetFrequency(float frequency);

  /// Sets the type of the oscillator.
  ///
  /// @param type Oscillator type.
  void SetType(OscillatorType type);

 private:
  // Inverse sampling rate in seconds.
  const float sample_interval_;

  // Oscillator type.
  OscillatorType type_;

  // Internal clock.
  float phase_;

  // Increment per sample.
  float increment_;

  // Random number generator (to generate white noise).
  RandomGenerator random_generator_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
