#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include <algorithm>
#include <cassert>

namespace barely::internal {

/// Envelope that generates output samples according to its current state.
class Envelope {
 public:
  /// ADSR (Attack-Decay-Sustain-Release).
  class Adsr {
   public:
    /// Constructs new `Adsr`.
    ///
    /// @param sample_interval Sample interval in seconds.
    explicit Adsr(double sample_interval) noexcept : sample_interval_(sample_interval) {}

    /// Sets the attack.
    ///
    /// @param attack Attack in seconds.
    void SetAttack(double attack) noexcept {
      attack_increment_ = (attack > 0.0) ? sample_interval_ / attack : 0.0;
      if (attack_increment_ > 1.0) {
        attack_increment_ = 0.0;
      }
    }

    /// Sets the decay.
    ///
    /// @param  decay Attack in seconds.
    void SetDecay(double decay) noexcept {
      decay_increment_ = (decay > 0.0) ? sample_interval_ / decay : 0.0;
      if (decay_increment_ > 1.0) {
        decay_increment_ = 0.0;
      }
    }

    /// Sets the release.
    ///
    /// @param  release Release in seconds.
    void SetRelease(double release) noexcept {
      release_decrement_ = (release > 0.0) ? -sample_interval_ / release : 0.0;
      if (release_decrement_ < -1.0) {
        release_decrement_ = 0.0;
      }
    }

    /// Sets the sustain of the envelope in amplitude.
    ///
    /// @param  sustain Sustain in amplitude range [0, 1].
    void SetSustain(double sustain) noexcept { sustain_ = std::clamp(sustain, 0.0, 1.0); }

   private:
    friend class Envelope;

    // Sample interval in seconds.
    double sample_interval_ = 0.0;

    // ADSR values.
    double attack_increment_ = 0.0;
    double decay_increment_ = 0.0;
    double sustain_ = 1.0;
    double release_decrement_ = 0.0;
  };

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return state_ != State::kIdle; }

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  double Next() noexcept {
    if (state_ == State::kIdle) {
      return 0.0;
    }
    assert(adsr_ != nullptr);
    if (state_ == State::kAttack) {
      if (adsr_->attack_increment_ > 0.0) {
        output_ = phase_;
        phase_ += adsr_->attack_increment_;
        if (phase_ >= 1.0) {
          phase_ = 0.0;
          state_ = State::kDecay;
        }
        return output_;
      }
      phase_ = 0.0;
      state_ = State::kDecay;
    }
    if (state_ == State::kDecay) {
      if (adsr_->decay_increment_ > 0.0) {
        output_ = 1.0 - phase_ * (1.0 - adsr_->sustain_);
        phase_ += adsr_->decay_increment_;
        if (phase_ >= 1.0) {
          phase_ = 0.0;
          state_ = State::kSustain;
        }
        return output_;
      }
      state_ = State::kSustain;
    }
    if (state_ == State::kSustain) {
      output_ = adsr_->sustain_;
      return output_;
    }
    if (state_ == State::kRelease) {
      if (adsr_->release_decrement_ < 0.0) {
        output_ = phase_ * release_output_;
        phase_ += adsr_->release_decrement_;
        if (phase_ <= 0.0) {
          phase_ = 0.0;
          state_ = State::kIdle;
        }
        return output_;
      }
      state_ = State::kIdle;
    }
    return 0.0;
  }

  /// Resets the state.
  void Reset() noexcept { state_ = State::kIdle; }

  /// Starts the envelope.
  ///
  /// @param adsr Adsr.
  void Start(const Adsr& adsr) noexcept {
    adsr_ = &adsr;
    output_ = adsr_->sustain_;
    phase_ = 0.0;
    state_ = State::kAttack;
  }

  /// Stops the envelope.
  void Stop() noexcept {
    if (state_ != State::kIdle && state_ != State::kRelease) {
      phase_ = 1.0;
      release_output_ = output_;
      state_ = State::kRelease;
    }
  }

 private:
  // Envelope state.
  enum class State { kAttack, kDecay, kSustain, kRelease, kIdle };

  // Pointer to adsr.
  const Adsr* adsr_ = nullptr;

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
