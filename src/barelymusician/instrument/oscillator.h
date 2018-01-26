#ifndef BARELYMUSICIAN_INSTRUMENT_OSCILLATOR_H_
#define BARELYMUSICIAN_INSTRUMENT_OSCILLATOR_H_

#include "barelymusician/instrument/unit_generator.h"

namespace barelyapi {

// Simple oscillator that generates output samples of basic waveforms.
class Oscillator : public UnitGenerator {
 public:
  // Oscillator type.
  enum class Type {
    kSine = 0,    // Sine wave.
    kSaw = 1,     // Sawtooth wave.
    kSquare = 2,  // Square wave.
    kNoise = 3,   // White noise.
  };

  // Constructs new |Oscillator|.
  //
  // @param sample_interval Sampling interval in seconds.
  explicit Oscillator(float sample_interval);

  // Sets the frequency of the oscillator.
  //
  // @param frequency Oscillator frequency in Hz.
  void SetFrequency(float frequency);

  // Sets the type of the oscillator.
  //
  // @param type Oscillator type.
  void SetType(Type type);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

 private:
  // Inverse sampling rate in seconds.
  const float sample_interval_;

  // Oscillator type.
  Type type_;

  // Frequency in Hz.
  float frequency_;

  // Internal clock.
  float phase_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_OSCILLATOR_H_
