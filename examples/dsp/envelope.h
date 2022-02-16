#ifndef EXAMPLES_DSP_ENVELOPE_H_
#define EXAMPLES_DSP_ENVELOPE_H_

#include "examples/dsp/generator.h"

namespace barelyapi {

/// Standard ADSR (Attack-Decay-Sustain-Release) envelope that generates output
/// samples according to its current state.
class Envelope : public Generator {
 public:
  /// Constructs new `Envelope`.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit Envelope(int sample_rate) noexcept;

  /// Implements `Generator`.
  float Next() noexcept override;
  void Reset() noexcept override;

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Sets the attack of the envelope in seconds.
  ///
  /// @param attack Attack in milliseconds.
  void SetAttack(float attack) noexcept;

  /// Sets the decay of the envelope in seconds.
  ///
  /// @param  decay Attack in seconds.
  void SetDecay(float decay) noexcept;

  /// Sets the release of the envelope in seconds.
  ///
  /// @param  release Release in seconds.
  void SetRelease(float release) noexcept;

  /// Sets the sustain of the envelope in amplitude.
  ///
  /// @param  sustain Sustain in amplitude range [0, 1].
  void SetSustain(float sustain) noexcept;

  /// Starts the envelope.
  void Start() noexcept;

  /// Stops the envelope.
  void Stop() noexcept;

 private:
  // Envelope state.
  enum class State { kAttack, kDecay, kSustain, kRelease, kIdle };

  // Inverse sampling rate in seconds.
  float sample_interval_;

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

#endif  // EXAMPLES_DSP_ENVELOPE_H_
