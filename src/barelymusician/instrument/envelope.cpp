#include "barelymusician/instrument/envelope.h"

#include <algorithm>

namespace barelyapi {

Envelope::Envelope(float sample_interval)
    : sample_interval_(sample_interval),
      attack_increment_(0.0f),
      decay_increment_(0.0f),
      sustain_(1.0f),
      release_increment_(0.0f),
      state_(State::kIdle),
      output_(0.0f),
      release_output_(0.0f),
      phase_(0.0f) {}

void Envelope::Start() {
  output_ = 0.0f;
  release_output_ = 0.0f;
  phase_ = 0.0f;
  state_ = State::kAttack;
}

void Envelope::Stop() {
  if (state_ != State::kIdle) {
    phase_ = 0.0f;
    release_output_ = output_;
    state_ = State::kRelease;
  }
}

void Envelope::SetAttack(float attack) {
  attack_increment_ = (attack > 0.0f) ? sample_interval_ / attack : 0.0f;
}

void Envelope::SetDecay(float decay) {
  decay_increment_ = (decay > 0.0f) ? sample_interval_ / decay : 0.0f;
}

void Envelope::SetSustain(float sustain) {
  sustain_ = std::min(std::max(sustain, 0.0f), 1.0f);
}

void Envelope::SetRelease(float release) {
  release_increment_ = (release > 0.0f) ? sample_interval_ / release : 0.0f;
}

float Envelope::Next() {
  switch (state_) {
    case State::kAttack:
      if (attack_increment_ > 0.0f) {
        output_ = phase_;
        phase_ += attack_increment_;
        if (phase_ >= 1.0f) {
          phase_ = 0.0f;
          state_ = State::kDecay;
        }
      } else {
        if (decay_increment_ > 0.0f) {
          output_ = 1.0f;
          phase_ = decay_increment_;
          state_ = State::kDecay;
        } else {
          output_ = sustain_;
          phase_ = 0.0f;
          state_ = State::kSustain;
        }
      }
      break;
    case State::kDecay:
      if (decay_increment_ > 0.0f) {
        output_ = 1.0f - phase_ * (1.0f - sustain_);
        phase_ += decay_increment_;
        if (phase_ >= 1.0f) {
          phase_ = 0.0f;
          state_ = State::kSustain;
        }
      } else {
        output_ = sustain_;
        phase_ = 0.0f;
        state_ = State::kSustain;
      }
      break;
    case State::kSustain:
      output_ = sustain_;
      break;
    case State::kRelease:
      output_ = (1.0f - phase_) * release_output_;
      phase_ += release_increment_;
      if (phase_ >= 1.0) {
        phase_ = 0.0f;
        state_ = State::kIdle;
      }
      break;
    case State::kIdle:
    default:
      return 0.0f;
  }
  return output_;
}

void Envelope::Reset() { state_ = State::kIdle; }

}  // namespace barelyapi
