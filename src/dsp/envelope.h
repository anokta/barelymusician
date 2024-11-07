#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

namespace barely::internal {

/// Standard ADSR (Attack-Decay-Sustain-Release) envelope that generates output samples according to
/// its current state.
class Envelope {
 public:
  /// Constructs new `Envelope`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Envelope(int sample_rate) noexcept;

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  double Next() noexcept;

  /// Resets the state.
  void Reset() noexcept;

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

  // Inverse sampling rate in seconds.
  double sample_interval_ = 0.0;

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

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
