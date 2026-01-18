#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include <algorithm>
#include <cassert>

namespace barely {

// Envelope that generates output samples according to its current state.
class Envelope {
 public:
  // Attack-Decay-Sustain-Release.
  class Adsr {
   public:
    void SetAttack(float sample_rate, float attack) noexcept {
      const float attack_samples = sample_rate * attack;
      attack_increment_ = (attack_samples >= 1.0f) ? 1.0f / attack_samples : 0.0f;
    }

    void SetDecay(float sample_rate, float decay) noexcept {
      const float decay_samples = sample_rate * decay;
      decay_increment_ = (decay_samples >= 1.0f) ? 1.0f / decay_samples : 0.0f;
    }

    void SetRelease(float sample_rate, float release) noexcept {
      const float release_samples = sample_rate * release;
      release_increment_ = (release_samples >= 1.0f) ? 1.0f / release_samples : 0.0f;
    }

    void SetSustain(float sustain) noexcept { sustain_ = std::clamp(sustain, 0.0f, 1.0f); }

   private:
    friend class Envelope;

    float attack_increment_ = 0.0f;
    float decay_increment_ = 0.0f;
    float sustain_ = 1.0f;
    float release_increment_ = 0.0f;
  };

  [[nodiscard]] constexpr bool IsActive() const noexcept { return state_ != State::kIdle; }

  [[nodiscard]] constexpr bool IsOn() const noexcept {
    return static_cast<int>(state_) < static_cast<int>(State::kRelease);
  }

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
      if (adsr_->release_increment_ > 0.0f) {
        output_ = phase_ * release_output_;
        phase_ -= adsr_->release_increment_;
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

  void Reset() noexcept { state_ = State::kIdle; }

  void Start(const Adsr& adsr) noexcept {
    adsr_ = &adsr;
    output_ = adsr_->sustain_;
    phase_ = 0.0f;
    state_ = State::kAttack;
  }

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
  enum class State { kAttack = 0, kDecay, kSustain, kRelease, kIdle };

  const Adsr* adsr_ = nullptr;

  State state_ = State::kIdle;

  float output_ = 0.0f;
  float release_output_ = 0.0f;

  float phase_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
