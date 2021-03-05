#include "barelymusician/engine/clock.h"

namespace barelyapi {

Clock::Clock()
    : is_playing_(false), position_(0.0), tempo_(1.0), timestamp_(0.0) {}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

double Clock::GetTimestamp() const { return timestamp_; }

double Clock::GetTimestampAtPosition(double position) const {
  return timestamp_ + (position - position_) / tempo_;
}

bool Clock::IsPlaying() const { return is_playing_; }

void Clock::SetPosition(double position) { position_ = position; }

void Clock::SetTempo(double tempo) { tempo_ = tempo; }

void Clock::Start() { is_playing_ = true; }

void Clock::Stop() { is_playing_ = false; }

void Clock::Update(double timestamp) {
  if (timestamp > timestamp_) {
    if (is_playing_) {
      position_ += tempo_ * (timestamp - timestamp_);
    }
    timestamp_ = timestamp;
  }
}

}  // namespace barelyapi
