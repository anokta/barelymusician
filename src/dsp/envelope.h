#ifndef BARELYMUSICIAN_DSP_ENVELOPE_H_
#define BARELYMUSICIAN_DSP_ENVELOPE_H_

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "core/control.h"

namespace barely {

// Envelope that generates output samples according to its current state.
class Envelope {
 public:
  // Attack-Decay-Sustain-Release.
  class Adsr {
   public:
    void SetAttack(double sample_rate, double attack) noexcept {
      attack_coeff_ = GetCoefficient(sample_rate, attack);
    }

    void SetDecay(double sample_rate, double decay) noexcept {
      decay_coeff_ = GetCoefficient(sample_rate, decay);
    }

    void SetRelease(double sample_rate, double release) noexcept {
      static constexpr double kMinRelease = 5e-3;  // 5ms
      release_coeff_ = GetCoefficient(sample_rate, std::max(release, kMinRelease));
    }

    void SetSustain(double sustain) noexcept { sustain_ = std::clamp(sustain, 0.0, 1.0); }

   private:
    friend class Envelope;

    double attack_coeff_ = 0.0;
    double decay_coeff_ = 0.0;
    double release_coeff_ = 0.0;
    double sustain_ = 1.0;
  };

  double Next() noexcept {
    assert(adsr_ != nullptr);

    switch (state_) {
      case State::kAttack:
        target_ = (adsr_->decay_coeff_ > 0.0) ? 1.0 : adsr_->sustain_;
        if (coeff_ == 0.0 || (current_ + kEnvelopeEpsilon) >= target_) {
          current_ = target_;
          target_ = adsr_->sustain_;
          if (adsr_->decay_coeff_ > 0.0) {
            state_ = State::kDecay;
            coeff_ = adsr_->decay_coeff_;
          } else {
            state_ = State::kSustain;
            coeff_ = 0.0;
          }
        }
        break;
      case State::kDecay:
        if (coeff_ == 0.0 || current_ <= (target_ + kEnvelopeEpsilon)) {
          current_ = target_;
          state_ = State::kSustain;
          coeff_ = 0.0;
        }
        break;
      case State::kRelease:
        if (current_ <= kEnvelopeEpsilon) {
          current_ = 0.0;
          state_ = State::kIdle;
          coeff_ = 0.0;
        }
        break;
      case State::kIdle:
        return 0.0;
      default:
        break;
    }

    const double output = current_;
    current_ = target_ + coeff_ * (current_ - target_);
    return output;
  }

  void Reset() noexcept {
    current_ = 0.0;
    target_ = 0.0;
    state_ = State::kIdle;
  }

  void Start(const Adsr& adsr) noexcept {
    adsr_ = &adsr;
    state_ = State::kAttack;
    target_ = (adsr_->decay_coeff_ > 0.0) ? 1.0 : adsr_->sustain_;
    coeff_ = adsr_->attack_coeff_;
  }

  void Stop() noexcept {
    if (state_ == State::kIdle || state_ == State::kRelease) {
      return;
    }
    if (state_ == State::kAttack && coeff_ == 0.0) {
      current_ = target_;
    }
    state_ = State::kRelease;
    target_ = 0.0;
    coeff_ = adsr_->release_coeff_;
  }

  [[nodiscard]] constexpr bool IsActive() const noexcept { return state_ != State::kIdle; }

  [[nodiscard]] constexpr bool IsOn() const noexcept { return state_ < State::kRelease; }

  [[nodiscard]] constexpr bool IsStartFrame() const noexcept {
    return state_ == State::kAttack && current_ == 0.0;
  }

 private:
  enum class State : uint8_t { kAttack = 0, kDecay, kSustain, kRelease, kIdle };

  const Adsr* adsr_ = nullptr;

  double current_ = 0.0;
  double target_ = 0.0;
  double coeff_ = 0.0;

  State state_ = State::kIdle;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
