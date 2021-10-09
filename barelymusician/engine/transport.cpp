#include "barelymusician/engine/transport.h"

#include <cmath>
#include <utility>

namespace barelyapi {

namespace {

// Dummy beat callback function that does nothing.
void NoopBeatCallback(double /*beat*/) {}

// Dummy update callback function that does nothing.
void NoopUpdateCallback(double /*begin_position*/, double /*end_position*/) {}

}  // namespace

Transport::Transport()
    : is_playing_(false),
      position_(0.0),
      tempo_(1.0),
      timestamp_(0.0),
      beat_callback_(&NoopBeatCallback),
      update_callback_(&NoopUpdateCallback) {}

double Transport::GetPosition() const { return position_; }

double Transport::GetPositionAtNextBeat() const {
  return (tempo_ < 0.0) ? std::floor(position_) : std::ceil(position_);
}

double Transport::GetTempo() const { return tempo_; }

double Transport::GetTimestamp() const { return timestamp_; }

double Transport::GetTimestampAtPosition(double position) const {
  return timestamp_ + (position - position_) / tempo_;
}

bool Transport::IsPlaying() const { return is_playing_; }

void Transport::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = beat_callback ? std::move(beat_callback) : &NoopBeatCallback;
}

void Transport::SetPosition(double position) { position_ = position; }

void Transport::SetTempo(double tempo) { tempo_ = tempo; }

void Transport::SetUpdateCallback(UpdateCallback update_callback) {
  update_callback_ =
      update_callback ? std::move(update_callback) : &NoopUpdateCallback;
}

void Transport::Start() { is_playing_ = true; }

void Transport::Stop() { is_playing_ = false; }

void Transport::Update(double timestamp) {
  while (timestamp_ < timestamp) {
    if (!is_playing_ || tempo_ == 0.0) {
      timestamp_ = timestamp;
      return;
    }
    // Compute next beat.
    double beat = GetPositionAtNextBeat();
    if (position_ == beat) {
      beat_callback_(position_);
      if (!is_playing_ || tempo_ == 0.0) {
        timestamp_ = timestamp;
        return;
      }
      beat = GetPositionAtNextBeat();
      if (position_ == beat) {
        beat = (tempo_ < 0.0) ? beat - 1.0 : beat + 1.0;
      }
    }
    const double beat_timestamp = GetTimestampAtPosition(beat);
    // Update position.
    const double begin_position = position_;
    if (beat_timestamp < timestamp) {
      timestamp_ = beat_timestamp;
      position_ = beat;
    } else {
      position_ += tempo_ * (timestamp - timestamp_);
      timestamp_ = timestamp;
    }
    update_callback_(begin_position, position_);
  }
}

}  // namespace barelyapi
