#include "barelymusician/engine/transport.h"

#include <cassert>
#include <cmath>
#include <utility>

namespace barelyapi {

double Transport::GetPosition() const noexcept { return position_; }

double Transport::GetTempo() const noexcept { return tempo_; }

double Transport::GetTimestamp() const noexcept { return timestamp_; }

double Transport::GetTimestamp(double position) const noexcept {
  assert(position >= 0.0);
  return timestamp_ + (position - position_) / tempo_;
}

bool Transport::IsPlaying() const noexcept { return is_playing_; }

void Transport::SetBeatCallback(BeatCallback beat_callback) noexcept {
  beat_callback_ = std::move(beat_callback);
}

void Transport::SetPosition(double position) noexcept {
  assert(position >= 0.0);
  if (position_ != position) {
    position_ = position;
    next_beat_position_ = std::ceil(position_);
    next_beat_timestamp_ = GetTimestamp(next_beat_position_);
  }
}

void Transport::SetTempo(double tempo) noexcept {
  assert(tempo >= 0.0);
  if (tempo_ != tempo) {
    tempo_ = tempo;
    next_beat_timestamp_ = GetTimestamp(next_beat_position_);
  }
}

void Transport::SetTimestamp(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (timestamp_ != timestamp) {
    timestamp_ = timestamp;
    next_beat_timestamp_ = GetTimestamp(next_beat_position_);
  }
}

void Transport::SetUpdateCallback(UpdateCallback update_callback) noexcept {
  update_callback_ = std::move(update_callback);
}

void Transport::Start() noexcept {
  next_beat_timestamp_ = GetTimestamp(next_beat_position_);
  is_playing_ = true;
}

void Transport::Stop() noexcept { is_playing_ = false; }

void Transport::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  while (timestamp_ < timestamp) {
    if (!is_playing_ || tempo_ <= 0.0) {
      timestamp_ = timestamp;
      return;
    }
    // Compute next beat.
    if (position_ == next_beat_position_) {
      if (beat_callback_) {
        beat_callback_(position_, timestamp_);
        if (!is_playing_ || tempo_ <= 0.0) {
          timestamp_ = timestamp;
          return;
        }
      }
      if (position_ == next_beat_position_) {
        next_beat_timestamp_ = GetTimestamp(++next_beat_position_);
      }
    }
    // Update position.
    const double begin_position = position_;
    if (next_beat_timestamp_ < timestamp) {
      position_ = next_beat_position_;
      timestamp_ = next_beat_timestamp_;
    } else {
      position_ += tempo_ * (timestamp - timestamp_);
      timestamp_ = timestamp;
    }
    if (update_callback_) {
      update_callback_(begin_position, position_);
    }
  }
}

}  // namespace barelyapi
