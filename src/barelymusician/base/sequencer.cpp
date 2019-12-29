#include "barelymusician/base/sequencer.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

Sequencer::Sequencer(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      beat_callback_(nullptr),
      tempo_(0.0f),
      beat_(0),
      sample_(0),
      num_samples_per_beat_(0) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

void Sequencer::Reset() {
  beat_ = 0;
  sample_ = 0;
}

void Sequencer::SetBeatCallback(BeatCallback&& beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Sequencer::SetTempo(float tempo) {
  DCHECK_GE(tempo, 0.0f);
  tempo_ = tempo;
  const float beat = BeatsFromSamples(sample_, num_samples_per_beat_);
  num_samples_per_beat_ =
      (tempo_ > 0.0f) ? static_cast<int>(num_samples_per_minute_ / tempo_) : 0;
  sample_ = SamplesFromBeats(beat, num_samples_per_beat_);
}

void Sequencer::Update(int num_samples) {
  DCHECK_GE(num_samples, 0);
  if (num_samples_per_beat_ == 0) {
    return;
  }
  sample_ += num_samples;
  if (sample_ == num_samples && beat_callback_ != nullptr) {
    beat_callback_(beat_, sample_);
  }
  while (sample_ >= num_samples_per_beat_) {
    ++beat_;
    sample_ -= num_samples_per_beat_;
    if (sample_ > 0 && beat_callback_ != nullptr) {
      beat_callback_(beat_, sample_);
    }
  }
}

}  // namespace barelyapi
