#ifndef BARELYMUSICIAN_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_DSP_OSCILLATOR_H_

#include <random>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/dsp/generator.h"

namespace barely {

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
  /// @param frame_rate Frame rate in hertz.
  explicit Oscillator(Integer frame_rate) noexcept;

  /// Implements `Generator`.
  double Next() noexcept override;
  void Reset() noexcept override;

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

  // Oscillator type.
  OscillatorType type_ = OscillatorType::kNoise;

  // Increment per sample.
  double increment_ = 0.0;

  // Internal clock.
  double phase_ = 0.0;

  // White noise random number generator.
  Random random_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_OSCILLATOR_H_
