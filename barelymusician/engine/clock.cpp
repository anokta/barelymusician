#include "barelymusician/engine/clock.h"

#include <utility>

#include "barelymusician/common/logging.h"

namespace barelyapi {

namespace {

// Dummy beat callback function that does nothing.
void NoopBeatCallback(double /*beat*/, double /*timestamp*/) {}

// Dummy update callback function that does nothing.
void NoopUpdateCallback(double /*begin_position*/, double /*end_position*/) {}

}  // namespace

Clock::Clock()
    : position_(0.0),
      tempo_(1.0),
      timestamp_(0.0),
      beat_callback_(&NoopBeatCallback),
      update_callback_(&NoopUpdateCallback) {}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

double Clock::GetTimestamp() const { return timestamp_; }

double Clock::GetTimestampAtPosition(double position) const {
  DCHECK_NE(tempo_, 0.0);
  return timestamp_ + (position - position_) / tempo_;
}

void Clock::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = beat_callback ? std::move(beat_callback) : &NoopBeatCallback;
}

void Clock::SetPosition(double position) { position_ = position; }

void Clock::SetTempo(double tempo) { tempo_ = tempo; }

void Clock::SetUpdateCallback(UpdateCallback update_callback) {
  update_callback_ =
      update_callback ? std::move(update_callback) : &NoopUpdateCallback;
}

void Clock::UpdatePosition(double timestamp) {
  double last_timestamp = -1.0;
  while (timestamp_ < timestamp) {
    if (tempo_ != 0.0) {
      const double begin_position = position_;
      double beat =
          (tempo_ > 0.0) ? std::ceil(position_) : std::floor(position_);
      double beat_timestamp = GetTimestampAtPosition(beat);
      if (beat_timestamp == last_timestamp) {
        beat = (tempo_ > 0.0) ? beat + 1.0 : beat - 1.0;
        beat_timestamp = GetTimestampAtPosition(beat);
      }
      last_timestamp = beat_timestamp;
      if (timestamp > beat_timestamp) {
        position_ = beat;
        timestamp_ = beat_timestamp;
        beat_callback_(beat, beat_timestamp);
      } else {
        position_ += tempo_ * (timestamp - timestamp_);
        timestamp_ = timestamp;
      }
      update_callback_(begin_position, position_);
    } else {
      timestamp_ = timestamp;
    }
  }
}

}  // namespace barelyapi
