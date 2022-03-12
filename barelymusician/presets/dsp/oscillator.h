#ifndef BARELYMUSICIAN_PRESETS_DSP_OSCILLATOR_H_
#define BARELYMUSICIAN_PRESETS_DSP_OSCILLATOR_H_

#include <random>

#include "barelymusician/api/presets/instruments.h"
#include "barelymusician/presets/dsp/generator.h"

namespace barelyapi {

/// Simple oscillator that generates output samples of basic waveforms.
class Oscillator : public Generator {
 public:
  /// Constructs new `Oscillator`.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit Oscillator(int sample_rate) noexcept;

  /// Implements `Generator`.
  double Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the frequency of the oscillator.
  ///
  /// @param frequency Oscillator frequency in hz.
  void SetFrequency(double frequency) noexcept;

  /// Sets the type of the oscillator.
  ///
  /// @param type Oscillator type.
  void SetType(barely::presets::OscillatorType type) noexcept;

 private:
  // Inverse sampling rate in seconds.
  double sample_interval_;

  // Oscillator type.
  barely::presets::OscillatorType type_;

  // Increment per sample.
  double increment_;

  // Internal clock.
  double phase_;

  // White noise random number generator.
  std::uniform_real_distribution<double> white_noise_distribution_;
  std::default_random_engine white_noise_engine_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_DSP_OSCILLATOR_H_
