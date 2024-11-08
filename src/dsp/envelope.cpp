#include "dsp/envelope.h"

#include <algorithm>

namespace barely::internal {

Envelope::Adsr::Adsr(int sample_rate) noexcept
    : sample_interval_((sample_rate > 0) ? 1.0 / static_cast<double>(sample_rate) : 0.0) {}

void Envelope::Adsr::SetAttack(double attack) noexcept {
  attack_increment_ = (attack > 0.0) ? sample_interval_ / attack : 0.0;
  if (attack_increment_ > 1.0) {
    attack_increment_ = 0.0;
  }
}

void Envelope::Adsr::SetDecay(double decay) noexcept {
  decay_increment_ = (decay > 0.0) ? sample_interval_ / decay : 0.0;
  if (decay_increment_ > 1.0) {
    decay_increment_ = 0.0;
  }
}

void Envelope::Adsr::SetRelease(double release) noexcept {
  release_increment_ = (release > 0.0) ? sample_interval_ / release : 0.0;
  if (release_increment_ > 1.0) {
    release_increment_ = 0.0;
  }
}

void Envelope::Adsr::SetSustain(double sustain) noexcept {
  sustain_ = std::min(std::max(sustain, 0.0), 1.0);
}

Envelope::Envelope(const Adsr& adsr) noexcept : adsr_(adsr) {}

bool Envelope::IsActive() const noexcept { return state_ != State::kIdle; }

double Envelope::Next() noexcept {
  if (state_ == State::kIdle) {
    return 0.0;
  }
  if (state_ == State::kAttack) {
    if (adsr_.attack_increment_ > 0.0) {
      output_ = phase_;
      phase_ += adsr_.attack_increment_;
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
    if (adsr_.decay_increment_ > 0.0) {
      output_ = 1.0 - phase_ * (1.0 - adsr_.sustain_);
      phase_ += adsr_.decay_increment_;
      if (phase_ >= 1.0) {
        phase_ = 0.0;
        state_ = State::kSustain;
      }
      return output_;
    }
    phase_ = 0.0;
    state_ = State::kSustain;
  }
  if (state_ == State::kSustain) {
    output_ = adsr_.sustain_;
    return output_;
  }
  if (state_ == State::kRelease) {
    if (adsr_.release_increment_ > 0.0) {
      output_ = (1.0 - phase_) * release_output_;
      phase_ += adsr_.release_increment_;
      if (phase_ >= 1.0) {
        phase_ = 0.0;
        state_ = State::kIdle;
      }
      return output_;
    }
    phase_ = 0.0;
    state_ = State::kIdle;
  }
  return 0.0;
}

void Envelope::Reset() noexcept { state_ = State::kIdle; }

void Envelope::Start() noexcept {
  phase_ = 0.0;
  if (adsr_.attack_increment_ > 0.0) {
    output_ = 0.0;
    state_ = State::kAttack;
  } else if (adsr_.decay_increment_ > 0.0) {
    output_ = 1.0;
    state_ = State::kDecay;
  } else {
    output_ = adsr_.sustain_;
    state_ = State::kSustain;
  }
}

void Envelope::Stop() noexcept {
  if (state_ != State::kIdle && state_ != State::kRelease) {
    phase_ = 0.0;
    release_output_ = output_;
    state_ = State::kRelease;
  }
}

}  // namespace barely::internal
