#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include "barelymusician/dsp/generator.h"

namespace barely {

/// Standard ADSR (Attack-Decay-Sustain-Release) envelope that generates output samples according to
/// its current state.
class Envelope : public Generator {
 public:
  /// Constructs new `Envelope`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Envelope(int frame_rate) noexcept;

  /// Implements `Generator`.
  double Next() noexcept override;
  void Reset() noexcept override;

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Sets the attack of the envelope in seconds.
  ///
  /// @param attack Attack in milliseconds.
  void SetAttack(double attack) noexcept;

  /// Sets the decay of the envelope in seconds.
  ///
  /// @param  decay Attack in seconds.
  void SetDecay(double decay) noexcept;

  /// Sets the release of the envelope in seconds.
  ///
  /// @param  release Release in seconds.
  void SetRelease(double release) noexcept;

  /// Sets the sustain of the envelope in amplitude.
  ///
  /// @param  sustain Sustain in amplitude range [0, 1].
  void SetSustain(double sustain) noexcept;

  /// Starts the envelope.
  void Start() noexcept;

  /// Stops the envelope.
  void Stop() noexcept;

 private:
  // Envelope state.
  enum class State { kAttack, kDecay, kSustain, kRelease, kIdle };

  // Inverse frame rate in seconds.
  double frame_interval_ = 0.0;

  // Current ADSR values.
  double attack_increment_ = 0.0;
  double decay_increment_ = 0.0;
  double sustain_ = 1.0;
  double release_increment_ = 0.0;

  // Last output value.
  double output_ = 0.0;

  // Last output value on release.
  double release_output_ = 0.0;

  // Internal clock.
  double phase_ = 0.0;

  // Current state.
  State state_ = State::kIdle;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
