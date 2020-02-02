#include "barelymusician/engine/clock.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

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

int Clock::GetNumSamplesPerBeat() const { return num_samples_per_beat_; }

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
