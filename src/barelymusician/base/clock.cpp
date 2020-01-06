#include "barelymusician/base/clock.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

Clock::Clock(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      beat_(0),
      leftover_samples_(0),
      num_samples_per_beat_(0),
      tempo_(0.0f) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

int Clock::GetBeat() const { return beat_; }

int Clock::GetLeftoverSamples() const { return leftover_samples_; }

int Clock::GetNumSamplesPerBeat() const { return num_samples_per_beat_; }

float Clock::GetTempo() const { return tempo_; }

void Clock::Reset() {
  beat_ = 0;
  leftover_samples_ = 0;
}

void Clock::SetTempo(float tempo) {
  DCHECK_GE(tempo, 0.0f);
  tempo_ = tempo;
  const float leftover_beats =
      BeatsFromSamples(leftover_samples_, num_samples_per_beat_);
  num_samples_per_beat_ =
      (tempo_ > 0.0f) ? static_cast<int>(num_samples_per_minute_ / tempo_) : 0;
  leftover_samples_ = SamplesFromBeats(leftover_beats, num_samples_per_beat_);
}

void Clock::Update(int num_samples) {
  DCHECK_GE(num_samples, 0);
  if (num_samples_per_beat_ == 0) {
    return;
  }
  leftover_samples_ += num_samples;
  beat_ += leftover_samples_ / num_samples_per_beat_;
  leftover_samples_ %= num_samples_per_beat_;
}

}  // namespace barelyapi
