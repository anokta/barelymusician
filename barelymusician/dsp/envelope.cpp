#include "barelymusician/dsp/envelope.h"

#include <cassert>

namespace barely {

Envelope::Envelope(int sample_rate)
    : sample_interval_(1.0f / static_cast<float>(sample_rate)),
      attack_increment_(0.0f),
      decay_increment_(0.0f),
      sustain_(1.0f),
      release_increment_(0.0f),
      state_(State::kIdle),
      output_(0.0f),
      release_output_(0.0f),
      phase_(0.0f) {
  assert(sample_rate > 0);
}

float Envelope::Next() {
  if (state_ == State::kIdle) {
    return 0.0f;
  }
  if (state_ == State::kAttack) {
    if (attack_increment_ > 0.0f) {
      output_ = phase_;
      phase_ += attack_increment_;
      if (phase_ >= 1.0f) {
        phase_ = 0.0f;
        state_ = State::kDecay;
      }
      return output_;
    } else {
      phase_ = 0.0f;
      state_ = State::kDecay;
    }
  }
  if (state_ == State::kDecay) {
    if (decay_increment_ > 0.0f) {
      output_ = 1.0f - phase_ * (1.0f - sustain_);
      phase_ += decay_increment_;
      if (phase_ >= 1.0f) {
        phase_ = 0.0f;
        state_ = State::kSustain;
      }
      return output_;
    } else {
      phase_ = 0.0f;
      state_ = State::kSustain;
    }
  }
  if (state_ == State::kSustain) {
    output_ = sustain_;
    return output_;
  }
  if (state_ == State::kRelease) {
    if (release_increment_ > 0.0f) {
      output_ = (1.0f - phase_) * release_output_;
      phase_ += release_increment_;
      if (phase_ >= 1.0) {
        phase_ = 0.0f;
        state_ = State::kIdle;
      }
      return output_;
    } else {
      phase_ = 0.0f;
      state_ = State::kIdle;
    }
  }
  return 0.0f;
}

void Envelope::Reset() { state_ = State::kIdle; }

bool Envelope::IsActive() const { return state_ != State::kIdle; }

void Envelope::SetAttack(float attack) {
  assert(attack >= 0.0f);
  attack_increment_ = (attack > 0.0f) ? sample_interval_ / attack : 0.0f;
  if (attack_increment_ > 1.0f) {
    attack_increment_ = 0.0f;
  }
}

void Envelope::SetDecay(float decay) {
  assert(decay >= 0.0f);
  decay_increment_ = (decay > 0.0f) ? sample_interval_ / decay : 0.0f;
  if (decay_increment_ > 1.0f) {
    decay_increment_ = 0.0f;
  }
}

void Envelope::SetRelease(float release) {
  assert(release >= 0.0f);
  release_increment_ = (release > 0.0f) ? sample_interval_ / release : 0.0f;
  if (release_increment_ > 1.0f) {
    release_increment_ = 0.0f;
  }
}

void Envelope::SetSustain(float sustain) {
  assert(sustain >= 0.0f && sustain <= 1.0f);
  sustain_ = sustain;
}

void Envelope::Start() {
  phase_ = 0.0f;
  if (attack_increment_ > 0.0) {
    output_ = 0.0f;
    state_ = State::kAttack;
  } else if (decay_increment_ > 0.0) {
    output_ = 1.0f;
    state_ = State::kDecay;
  } else {
    output_ = sustain_;
    state_ = State::kSustain;
  }
}

void Envelope::Stop() {
  if (state_ != State::kIdle) {
    phase_ = 0.0f;
    release_output_ = output_;
    state_ = State::kRelease;
  }
}

}  // namespace barely
