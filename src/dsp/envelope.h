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
  // Attack-Decay-Release.
  class Adr {
   public:
    void SetAttack(float sample_rate, float attack) noexcept {
      attack_coeff_ = GetCoefficient(sample_rate, attack);
    }

    void SetDecay(float sample_rate, float decay) noexcept {
      decay_coeff_ = GetCoefficient(sample_rate, decay);
    }

    void SetRelease(float sample_rate, float release) noexcept {
      static constexpr float kMinRelease = 5e-3f;  // 5ms
      release_coeff_ = GetCoefficient(sample_rate, std::max(release, kMinRelease));
    }

   private:
    friend class Envelope;
    float attack_coeff_ = 0.0f;
    float decay_coeff_ = 0.0f;
    float release_coeff_ = 0.0f;
  };

  float Next(const Adr& adr, float sustain) noexcept {
    float target = 0.0f;
    float coeff = 0.0f;

    switch (state_) {
      case State::kAttack:
        target = (adr.decay_coeff_ > 0.0f) ? 1.0f : sustain;
        if (adr.attack_coeff_ == 0.0f || (current_ + kEnvelopeEpsilon) >= target) {
          current_ = target;
          target = sustain;
          if (adr.decay_coeff_ > 0.0f) {
            state_ = State::kDecay;
            coeff = adr.decay_coeff_;
          } else {
            state_ = State::kSustain;
            coeff = 0.0f;
          }
        } else {
          coeff = adr.attack_coeff_;
        }
        break;
      case State::kDecay:
        if (adr.decay_coeff_ == 0.0f || current_ <= (sustain + kEnvelopeEpsilon)) {
          current_ = sustain;
          target = sustain;
          state_ = State::kSustain;
          coeff = 0.0f;
        } else {
          coeff = adr.decay_coeff_;
        }
        break;
      case State::kSustain:
        target = sustain;
        break;
      case State::kRelease:
        if (current_ <= kEnvelopeEpsilon) {
          current_ = 0.0f;
          state_ = State::kIdle;
        } else {
          coeff = adr.release_coeff_;
        }
        break;
      case State::kIdle:
        return 0.0f;
      default:
        break;
    }

    const float output = current_;
    current_ = target + coeff * (current_ - target);
    return output;
  }

  void Reset() noexcept {
    current_ = 0.0f;
    state_ = State::kIdle;
  }

  void Start() noexcept { state_ = State::kAttack; }

  void Stop(const Adr& adr, float sustain) noexcept {
    if (state_ == State::kIdle || state_ == State::kRelease) {
      return;
    }
    if (state_ == State::kAttack && adr.attack_coeff_ == 0.0f) {
      current_ = (adr.decay_coeff_ > 0.0f) ? 1.0f : sustain;
    }
    state_ = State::kRelease;
  }

  [[nodiscard]] constexpr bool IsActive() const noexcept { return state_ != State::kIdle; }

  [[nodiscard]] constexpr bool IsOn() const noexcept { return state_ < State::kRelease; }

  [[nodiscard]] constexpr bool IsStartFrame() const noexcept {
    return state_ == State::kAttack && current_ == 0.0f;
  }

 private:
  enum class State : uint8_t { kAttack = 0, kDecay, kSustain, kRelease, kIdle };
  float current_ = 0.0f;
  State state_ = State::kIdle;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPE_H_
