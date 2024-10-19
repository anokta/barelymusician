#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <random>

#include "barelymusician/common/random.h"

namespace barely {

/// Oscillator type.
enum class OscillatorType {
  /// None.
  kNone = 0,
  /// Sine wave.
  kSine,
  /// Sawtooth wave.
  kSaw,
  /// Square wave.
  kSquare,
  /// White noise.
  kNoise,
  /// Count.
  kCount,
};

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator {
 public:
  /// Constructs new `Oscillator`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Oscillator(int frame_rate) noexcept;

  /// Returns the frequency of the oscillator.
  ///
  /// @return Frequency in hertz.
  [[nodiscard]] double GetFrequency() const noexcept;

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  [[nodiscard]] double Next() noexcept;

  /// Resets the state.
  void Reset() noexcept;

  /// Sets the frequency of the oscillator.
  ///
  /// @param frequency Oscillator frequency in hertz.
  void SetFrequency(double frequency) noexcept;

  /// Sets the type of the oscillator.
  ///
  /// @param type Oscillator type.
  void SetType(OscillatorType type) noexcept;

 private:
  // Inverse frame rate in seconds.
  double frame_interval_ = 0.0;

  // Maximum allowed frequency.
  double max_frequency_ = 0.0;

  // Oscillator type.
  OscillatorType type_ = OscillatorType::kNone;

  // Frequency.
  double frequency_ = 0.0;

  // Increment per sample.
  double increment_ = 0.0;

  // Internal clock.
  double phase_ = 0.0;

  // White noise random number generator.
  Random random_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
