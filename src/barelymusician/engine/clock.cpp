#include "barelymusician/engine/clock.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Clock::Clock(int sample_rate)
    : num_samples_per_minute_(static_cast<double>(sample_rate) *
                              kSecondsFromMinutes),
      beat_(0),
      leftover_samples_(0),
      num_samples_per_beat_(0),
      position_(0.0),
      tempo_(0.0) {
  DCHECK_GE(sample_rate, 0);
}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

void Clock::SetPosition(double position) {
  DCHECK_GE(position, 0.0);
  position_ = position;
  const double beat = std::floor(position_);
  beat_ = static_cast<int>(beat);
  leftover_samples_ = SamplesFromBeats(position_ - beat);
}

void Clock::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  tempo_ = tempo;
  num_samples_per_beat_ =
      (tempo_ > 0.0) ? static_cast<int>(num_samples_per_minute_ / tempo_) : 0;
  leftover_samples_ = SamplesFromBeats(position_ - std::floor(position_));
}

void Clock::UpdatePosition(int num_samples) {
  DCHECK_GE(num_samples, 0);
  if (num_samples_per_beat_ == 0) {
    return;
  }
  leftover_samples_ += num_samples;
  beat_ += leftover_samples_ / num_samples_per_beat_;
  leftover_samples_ %= num_samples_per_beat_;
  position_ = static_cast<double>(beat_) + BeatsFromSamples(leftover_samples_);
}

double Clock::BeatsFromSamples(int samples) const {
  return (num_samples_per_beat_ > 0)
             ? static_cast<double>(samples) /
                   static_cast<double>(num_samples_per_beat_)
             : 0.0;
}

int Clock::SamplesFromBeats(double beats) const {
  return static_cast<int>(beats * static_cast<double>(num_samples_per_beat_));
}

}  // namespace barelyapi
