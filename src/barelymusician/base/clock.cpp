#include "barelymusician/base/clock.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

Clock::Clock(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      beat_callback_(nullptr),
      tempo_(0.0f),
      num_samples_per_beat_(0),
      beat_(0),
      leftover_samples_(0) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

int Clock::GetBeat() const { return beat_; }

int Clock::GetLeftoverSamples() const { return leftover_samples_; }

int Clock::GetNumSamplesPerBeat() const { return num_samples_per_beat_; }

float Clock::GetPosition() const {
  return static_cast<float>(beat_) +
         BeatsFromSamples(leftover_samples_, num_samples_per_beat_);
}

float Clock::GetTempo() const { return tempo_; }

void Clock::SetBeatCallback(BeatCallback&& beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Clock::SetPosition(float beat) {
  DCHECK_GE(beat, 0.0f);
  beat_ = static_cast<int>(beat);
  // TODO(#56): This will ignore the fractional part when
  // |num_samples_per_beat_| is zero (i.e., when |tempo_| is zero/unset).
  leftover_samples_ =
      SamplesFromBeats(beat - static_cast<float>(beat_), num_samples_per_beat_);
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
  if (leftover_samples_ == num_samples && beat_callback_ != nullptr) {
    beat_callback_(beat_, leftover_samples_);
  }
  while (leftover_samples_ >= num_samples_per_beat_) {
    ++beat_;
    leftover_samples_ -= num_samples_per_beat_;
    if (leftover_samples_ > 0 && beat_callback_ != nullptr) {
      beat_callback_(beat_, leftover_samples_);
    }
  }
}

}  // namespace barelyapi
