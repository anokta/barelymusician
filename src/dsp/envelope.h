#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include <algorithm>
#include <cassert>

#include "core/control.h"

namespace barely {

// Envelope that generates output samples according to its current state.
class Envelope {
 public:
  // Attack-Decay-Sustain-Release.
  class Adsr {
   public:
    void SetAttack(float sample_rate, float attack) noexcept {
      attack_coeff_ = GetEnvelopeCoefficient(sample_rate, attack);
    }

    void SetDecay(float sample_rate, float decay) noexcept {
      decay_coeff_ = GetEnvelopeCoefficient(sample_rate, decay);
    }

    void SetRelease(float sample_rate, float release) noexcept {
      static constexpr float kMinRelease = 5e-3f;  // 5ms
      release_coeff_ = GetEnvelopeCoefficient(sample_rate, std::max(release, kMinRelease));
    }

    void SetSustain(float sustain) noexcept { sustain_ = std::clamp(sustain, 0.0f, 1.0f); }

   private:
    friend class Envelope;

    float attack_coeff_ = 0.0f;
    float decay_coeff_ = 0.0f;
    float release_coeff_ = 0.0f;
    float sustain_ = 1.0f;
  };

  float Next() noexcept {
    assert(adsr_ != nullptr);

    switch (state_) {
      case State::kAttack:
        target_ = (adsr_->decay_coeff_ > 0.0f) ? 1.0f : adsr_->sustain_;
        if (coeff_ == 0.0f || (current_ + kEnvelopeEpsilon) >= target_) {
          current_ = target_;
          target_ = adsr_->sustain_;
          if (adsr_->decay_coeff_ > 0.0f) {
            state_ = State::kDecay;
            coeff_ = adsr_->decay_coeff_;
          } else {
            state_ = State::kSustain;
            coeff_ = 0.0f;
          }
        }
        break;
      case State::kDecay:
        if (coeff_ == 0.0f || current_ <= (target_ + kEnvelopeEpsilon)) {
          current_ = target_;
          state_ = State::kSustain;
          coeff_ = 0.0f;
        }
        break;
      case State::kRelease:
        if (current_ <= kEnvelopeEpsilon) {
          current_ = 0.0f;
          state_ = State::kIdle;
          coeff_ = 0.0f;
        }
        break;
      case State::kIdle:
        return 0.0f;
      default:
        break;
    }

    const float output = current_;
    current_ = target_ + coeff_ * (current_ - target_);
    return output;
  }

  void Reset() noexcept {
    current_ = 0.0f;
    target_ = 0.0f;
    state_ = State::kIdle;
  }

  void Start(const Adsr& adsr) noexcept {
    adsr_ = &adsr;
    state_ = State::kAttack;
    current_ = 0.0f;
    target_ = 1.0f;
    coeff_ = adsr_->attack_coeff_;
  }

  void Stop() noexcept {
    if (state_ == State::kIdle || state_ == State::kRelease) {
      return;
    }
    state_ = State::kRelease;
    target_ = 0.0f;
    coeff_ = adsr_->release_coeff_;
  }

  [[nodiscard]] constexpr bool IsActive() const noexcept { return state_ != State::kIdle; }

  [[nodiscard]] constexpr bool IsStartFrame() const noexcept {
    return state_ == State::kAttack && current_ == 0.0f;
  }

 private:
  enum class State { kAttack = 0, kDecay, kSustain, kRelease, kIdle };

  const Adsr* adsr_ = nullptr;

  State state_ = State::kIdle;

  float current_ = 0.0f;
  float target_ = 0.0f;
  float coeff_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
