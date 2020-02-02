#include "barelymusician/engine/clock.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Returns the corresponding number of beats for the given samples.
//
// @param samples Number of samples.
// @param num_samples_per_beat Number of samples per beat.
// @return Number of beats.
double BeatsFromSamples(int samples, int num_samples_per_beat) {
  return (num_samples_per_beat > 0)
             ? static_cast<double>(samples) /
                   static_cast<double>(num_samples_per_beat)
             : 0.0;
}

// Returns the corresponding number of samples for the given beats.
//
// @param beats Number of beats.
// @param num_samples_per_beat Number of samples per beat.
// @return Number of samples.
int SamplesFromBeats(double beats, int num_samples_per_beat) {
  return static_cast<int>(beats * static_cast<double>(num_samples_per_beat));
}

}  // namespace

Clock::Clock(int sample_rate)
    : num_samples_per_minute_(static_cast<double>(sample_rate) *
                              kSecondsFromMinutes),
      beat_(0),
      leftover_beats_(0.0),
      leftover_samples_(0),
      num_samples_per_beat_(0),
      tempo_(0.0) {
  DCHECK_GE(sample_rate, 0);
}

double Clock::GetPosition() const {
  return static_cast<double>(beat_) + leftover_beats_;
}

double Clock::GetTempo() const { return tempo_; }

void Clock::SetPosition(double position) {
  DCHECK_GE(position, 0.0);
  const double beat = std::floor(position);
  beat_ = static_cast<int>(beat);
  leftover_beats_ = position - beat;
  leftover_samples_ = SamplesFromBeats(leftover_beats_, num_samples_per_beat_);
}

void Clock::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  tempo_ = tempo;
  num_samples_per_beat_ =
      (tempo_ > 0.0) ? static_cast<int>(num_samples_per_minute_ / tempo_) : 0;
  leftover_samples_ = SamplesFromBeats(leftover_beats_, num_samples_per_beat_);
}

void Clock::UpdatePosition(int num_samples) {
  DCHECK_GE(num_samples, 0);
  if (num_samples_per_beat_ == 0) {
    return;
  }
  leftover_samples_ += num_samples;
  beat_ += leftover_samples_ / num_samples_per_beat_;
  leftover_samples_ %= num_samples_per_beat_;
  leftover_beats_ = BeatsFromSamples(leftover_samples_, num_samples_per_beat_);
}

}  // namespace barelyapi
