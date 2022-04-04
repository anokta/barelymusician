#include "barelymusician/engine/transport.h"

#include <cassert>
#include <cmath>
#include <utility>

namespace barelyapi {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

double Transport::GetPosition() const noexcept { return position_; }

double Transport::GetTempo() const noexcept { return tempo_; }

double Transport::GetTimestamp() const noexcept { return timestamp_; }

double Transport::GetTimestamp(double position) const noexcept {
  assert(tempo_ > 0.0);
  return timestamp_ + (position - position_) * kSecondsFromMinutes / tempo_;
}

bool Transport::IsPlaying() const noexcept { return is_playing_; }

void Transport::SetBeatCallback(BeatCallback callback) noexcept {
  beat_callback_ = std::move(callback);
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

void Transport::Start() noexcept {
  next_beat_timestamp_ = GetTimestamp(next_beat_position_);
  is_playing_ = true;
}

void Transport::Stop() noexcept { is_playing_ = false; }

void Transport::Update(double timestamp,
                       const UpdateCallback& callback) noexcept {
  assert(timestamp >= 0.0);
  while (timestamp_ < timestamp) {
    if (!is_playing_ || tempo_ == 0.0) {
      timestamp_ = timestamp;
      return;
    }
    // Compute next beat.
    if (position_ == next_beat_position_) {
      if (beat_callback_) {
        beat_callback_(position_, timestamp_);
        if (!is_playing_ || tempo_ == 0.0) {
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
      position_ += tempo_ * (timestamp - timestamp_) * kMinutesFromSeconds;
      timestamp_ = timestamp;
    }
    if (callback) {
      callback(begin_position, position_);
    }
  }
}

}  // namespace barelyapi
