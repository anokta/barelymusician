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

  // Inverse frame rate in seconds.
  float frame_interval_ = 0.0f;

  // Current ADSR values.
  float attack_increment_ = 0.0f;
  float decay_increment_ = 0.0f;
  float sustain_ = 1.0f;
  float release_increment_ = 0.0f;

  // Last output value.
  float output_ = 0.0f;

  // Last output value on release.
  float release_output_ = 0.0f;

  // Internal clock.
  float phase_ = 0.0f;

  // Current state.
  State state_ = State::kIdle;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
