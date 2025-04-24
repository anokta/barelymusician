#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include <algorithm>
#include <cassert>

namespace barely {

/// Envelope that generates output samples according to its current state.
class Envelope {
 public:
  /// ADSR (Attack-Decay-Sustain-Release).
  class Adsr {
   public:
    /// Sets the attack.
    ///
    /// @param sample_interval Sample interval in seconds.
    /// @param attack Attack in seconds.
    void SetAttack(float sample_interval, float attack) noexcept {
      attack_increment_ = (attack > 0.0f) ? sample_interval / attack : 0.0f;
      if (attack_increment_ > 1.0f) {
        attack_increment_ = 0.0f;
      }
    }

    /// Sets the decay.
    ///
    /// @param sample_interval Sample interval in seconds.
    /// @param decay Attack in seconds.
    void SetDecay(float sample_interval, float decay) noexcept {
      decay_increment_ = (decay > 0.0f) ? sample_interval / decay : 0.0f;
      if (decay_increment_ > 1.0f) {
        decay_increment_ = 0.0f;
      }
    }

    /// Sets the release.
    ///
    /// @param sample_interval Sample interval in seconds.
    /// @param release Release in seconds.
    void SetRelease(float sample_interval, float release) noexcept {
      release_decrement_ = (release > 0.0f) ? -sample_interval / release : 0.0f;
      if (release_decrement_ < -1.0f) {
        release_decrement_ = 0.0f;
      }
    }

    /// Sets the sustain of the envelope in amplitude.
    ///
    /// @param sustain Sustain in amplitude range [0, 1].
    void SetSustain(float sustain) noexcept { sustain_ = std::clamp(sustain, 0.0f, 1.0f); }

   private:
    friend class Envelope;

    // ADSR values.
    float attack_increment_ = 0.0f;
    float decay_increment_ = 0.0f;
    float sustain_ = 1.0f;
    float release_decrement_ = 0.0f;
  };

  /// Returns whether the envelope is currently active (i.e., not idle).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return state_ != State::kIdle; }

  /// Returns whether the envelope is currently on (i.e., not idle or released).
  ///
  /// @return True if on.
  [[nodiscard]] bool IsOn() const noexcept {
    return state_ != State::kIdle && state_ != State::kRelease;
  }

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  float Next() noexcept {
    if (state_ == State::kIdle) {
      return 0.0;
    }
    assert(adsr_ != nullptr);
    if (state_ == State::kAttack) {
      if (adsr_->attack_increment_ > 0.0f) {
        output_ = phase_;
        phase_ += adsr_->attack_increment_;
        if (phase_ >= 1.0f) {
          phase_ = 0.0f;
          state_ = State::kDecay;
        }
        return output_;
      }
      phase_ = 0.0f;
      state_ = State::kDecay;
    }
    if (state_ == State::kDecay) {
      if (adsr_->decay_increment_ > 0.0f) {
        output_ = 1.0f - phase_ * (1.0f - adsr_->sustain_);
        phase_ += adsr_->decay_increment_;
        if (phase_ >= 1.0f) {
          phase_ = 0.0f;
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
      if (adsr_->release_decrement_ < 0.0f) {
        output_ = phase_ * release_output_;
        phase_ += adsr_->release_decrement_;
        if (phase_ <= 0.0f) {
          phase_ = 0.0f;
          state_ = State::kIdle;
        }
        return output_;
      }
      state_ = State::kIdle;
    }
    return 0.0f;
  }

  /// Resets the state.
  void Reset() noexcept { state_ = State::kIdle; }

  /// Starts the envelope.
  ///
  /// @param adsr Adsr.
  void Start(const Adsr& adsr) noexcept {
    adsr_ = &adsr;
    output_ = adsr_->sustain_;
    phase_ = 0.0f;
    state_ = State::kAttack;
  }

  /// Stops the envelope.
  void Stop() noexcept {
    if (state_ == State::kIdle || state_ == State::kRelease) {
      return;
    }
    if (state_ == State::kAttack && phase_ == 0.0f && adsr_->attack_increment_ > 0.0f) {
      state_ = State::kIdle;
    } else {
      phase_ = 1.0f;
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
