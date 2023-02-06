#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/generator.h"

namespace barely {

/// Standard ADSR (Attack-Decay-Sustain-Release) envelope that generates output
/// samples according to its current state.
class Envelope : public Generator {
 public:
  /// Constructs new `Envelope`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Envelope(Integer frame_rate) noexcept;

  /// Implements `Generator`.
  Real Next() noexcept override;
  void Reset() noexcept override;

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Sets the attack of the envelope in seconds.
  ///
  /// @param attack Attack in milliseconds.
  void SetAttack(Real attack) noexcept;

  /// Sets the decay of the envelope in seconds.
  ///
  /// @param  decay Attack in seconds.
  void SetDecay(Real decay) noexcept;

  /// Sets the release of the envelope in seconds.
  ///
  /// @param  release Release in seconds.
  void SetRelease(Real release) noexcept;

  /// Sets the sustain of the envelope in amplitude.
  ///
  /// @param  sustain Sustain in amplitude range [0, 1].
  void SetSustain(Real sustain) noexcept;

  /// Starts the envelope.
  void Start() noexcept;

  /// Stops the envelope.
  void Stop() noexcept;

 private:
  // Envelope state.
  enum class State { kAttack, kDecay, kSustain, kRelease, kIdle };

  // Inverse frame rate in seconds.
  Real frame_interval_ = 0.0;

  // Current ADSR values.
  Real attack_increment_ = 0.0;
  Real decay_increment_ = 0.0;
  Real sustain_ = 1.0;
  Real release_increment_ = 0.0;

  // Last output value.
  Real output_ = 0.0;

  // Last output value on release.
  Real release_output_ = 0.0;

  // Internal clock.
  Real phase_ = 0.0;

  // Current state.
  State state_ = State::kIdle;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
