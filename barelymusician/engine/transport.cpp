#include "barelymusician/engine/transport.h"

#include <cassert>
#include <cmath>
#include <utility>

namespace barelyapi {

double Transport::GetPosition() const noexcept { return position_; }

bool Transport::IsPlaying() const noexcept { return is_playing_; }

void Transport::SetBeatCallback(BeatCallback callback) noexcept {
  beat_callback_ = std::move(callback);
}

void Transport::SetPosition(double position) noexcept {
  assert(position >= 0.0);
  if (position_ != position) {
    position_ = position;
    next_beat_position_ = std::ceil(position_);
  }
}

void Transport::Start() noexcept { is_playing_ = true; }

void Transport::Stop() noexcept { is_playing_ = false; }

void Transport::Update(double duration,
                       const UpdateCallback& callback) noexcept {
  assert(duration >= 0.0);
  while (duration > 0.0) {
    if (!is_playing_) return;
    // Compute next beat.
    if (position_ == next_beat_position_) {
      if (beat_callback_) {
        beat_callback_(position_);
        if (!is_playing_) return;
      }
      if (position_ == next_beat_position_) ++next_beat_position_;
    }
    // Update position.
    const double begin_position = position_;
    position_ = std::min(position_ + duration, next_beat_position_);
    duration -= position_ - begin_position;
    if (callback) {
      callback(begin_position, position_);
    }
  }
}

}  // namespace barelyapi
