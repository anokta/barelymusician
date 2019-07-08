#include "barelymusician/base/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

Sequencer::Sequencer(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      beat_callback_(nullptr),
      leftover_samples_(0),
      num_samples_per_beat_(0),
      transport_({}) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

const Transport& Sequencer::GetTransport() const { return transport_; }

void Sequencer::Reset() {
  transport_.section = 0;
  transport_.bar = 0;
  transport_.beat = 0;
  leftover_samples_ = 0;
}

void Sequencer::SetBeatCallback(BeatCallback&& beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Sequencer::SetNumBars(int num_bars) {
  DCHECK_GE(num_bars, 0);
  transport_.num_bars = num_bars;
  if (transport_.num_bars > 0) {
    transport_.bar = std::min(transport_.bar, transport_.num_bars - 1);
  }
}

void Sequencer::SetNumBeats(int num_beats) {
  DCHECK_GE(num_beats, 0);
  transport_.num_beats = num_beats;
  if (transport_.num_beats > 0) {
    transport_.beat = std::min(transport_.beat, transport_.num_beats - 1);
  }
}

void Sequencer::SetTempo(float tempo) {
  DCHECK_GE(tempo, 0.0f);
  transport_.tempo = tempo;
  const float leftover_beats =
      BeatsFromSamples(leftover_samples_, num_samples_per_beat_);
  num_samples_per_beat_ =
      (transport_.tempo > 0.0f)
          ? static_cast<int>(num_samples_per_minute_ / transport_.tempo)
          : 0;
  leftover_samples_ = SamplesFromBeats(leftover_beats, num_samples_per_beat_);
}

void Sequencer::Update(int num_samples) {
  if (num_samples_per_beat_ == 0) {
    return;
  }
  leftover_samples_ += num_samples;
  if (leftover_samples_ == num_samples && beat_callback_ != nullptr) {
    beat_callback_(transport_, 0, num_samples_per_beat_);
  }
  while (leftover_samples_ >= num_samples_per_beat_) {
    // Update beat count.
    ++transport_.beat;
    leftover_samples_ -= num_samples_per_beat_;
    if (transport_.num_beats > 0 && transport_.beat >= transport_.num_beats) {
      // Update bar count.
      ++transport_.bar;
      transport_.beat -= transport_.num_beats;
      if (transport_.num_bars > 0 && transport_.bar >= transport_.num_bars) {
        // Update section count.
        ++transport_.section;
        transport_.bar -= transport_.num_bars;
      }
    }
    if (leftover_samples_ > 0 && beat_callback_ != nullptr) {
      beat_callback_(transport_, num_samples - leftover_samples_,
                     num_samples_per_beat_);
    }
  }
}

}  // namespace barelyapi
