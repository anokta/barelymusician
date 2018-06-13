#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include "barelymusician/base/unit_generator.h"

namespace barelyapi {

// Standard ADSR (Attack-Decay-Sustain-Release) envelope that generates output
// samples according to its current state.
class Envelope : UnitGenerator {
 public:
  // Constructs new |Envelope|.
  //
  // @param sample_interval Sampling interval in seconds.
  explicit Envelope(float sample_interval);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Returns whether the envelope is currently active (i.e., not idle).
  //
  // @return True if active.
  bool IsActive() const;

  // Sets the attack of the envelope in seconds.
  //
  // attack Attack in milliseconds.
  void SetAttack(float attack);

  // Sets the decay of the envelope in seconds.
  //
  // decay Attack in seconds.
  void SetDecay(float decay);

  // Sets the release of the envelope in seconds.
  //
  // release Release in seconds.
  void SetRelease(float release);

  // Sets the sustain of the envelope in amplitude.
  //
  // sustain Sustain in amplitude range [0, 1].
  void SetSustain(float sustain);

  // Starts the envelope.
  void Start();

  // Stops the envelope.
  void Stop();

 private:
  // Envelope state.
  enum class State { kAttack, kDecay, kSustain, kRelease, kIdle };

  // Inverse sampling rate in seconds.
  const float sample_interval_;

  // Current ADSR values.
  float attack_increment_;
  float decay_increment_;
  float sustain_;
  float release_increment_;

  // Current state.
  State state_;

  // Last output value.
  float output_;

  // Last output value on release.
  float release_output_;

  // Internal clock.
  float phase_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
