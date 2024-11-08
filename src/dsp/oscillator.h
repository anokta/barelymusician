#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <random>

#include "barelymusician.h"
#include "common/random.h"

namespace barely::internal {

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator {
 public:
  /// Constructs new `Oscillator`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Oscillator(int sample_rate) noexcept;

  /// Generates the next output sample.
  ///
  /// @param shape Oscillator shape.
  /// @return Next output sample.
  [[nodiscard]] double Next(OscillatorShape shape) noexcept;

  /// Resets the state.
  void Reset() noexcept;

  /// Sets the frequency of the oscillator.
  ///
  /// @param frequency Oscillator frequency in hertz.
  void SetFrequency(double frequency) noexcept;

 private:
  // Inverse sampling rate in seconds.
  double sample_interval_ = 0.0;

  // Maximum allowed frequency.
  double max_frequency_ = 0.0;

  // Frequency.
  double frequency_ = 0.0;

  // Increment per sample.
  double increment_ = 0.0;

  // Internal clock.
  double phase_ = 0.0;

  // White noise random number generator.
  Random random_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
