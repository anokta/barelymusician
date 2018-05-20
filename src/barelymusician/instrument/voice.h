#ifndef BARELYMUSICIAN_INSTRUMENT_VOICE_H_
#define BARELYMUSICIAN_INSTRUMENT_VOICE_H_

#include "barelymusician/instrument/envelope.h"
#include "barelymusician/instrument/oscillator.h"
#include "barelymusician/instrument/unit_generator.h"

namespace barelyapi {

// Voice for a virtual instrument that wraps a unit generator with an envelope
// and a corresponding gain to generate output samples.
class Voice : public UnitGenerator {
 public:
  // Constructs new |Voice|.
  //
  // @param sample_interval Sampling interval in seconds.
  explicit Voice(float sample_interval);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Sets the gain of the voice.
  //
  // @param gain Gain in amplitude.
  void SetGain(float gain);

  // Starts the voice.
  void Start();

  // Stops the voice.
  void Stop();

  Envelope& envelope() { return envelope_; }
  const Envelope& envelope() const { return envelope_; }

  Oscillator& oscillator() { return oscillator_; }
  const Oscillator& oscillator() const { return oscillator_; }

 private:
  // Envelope.
  Envelope envelope_;

  // Unit generator.
  // TODO(#1): This should probably be generalized to |UnitGenerator*| at some
  // point (e.g., to support sample players).
  Oscillator oscillator_;

  // Gain in amplitude.
  float gain_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_VOICE_H_
