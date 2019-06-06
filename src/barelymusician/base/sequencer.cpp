#include "barelymusician/base/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

Sequencer::Sequencer(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      num_samples_per_beat_(0),
      leftover_samples_(0),
      transport_({}) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

const Transport& Sequencer::GetTransport() const { return transport_; }

void Sequencer::RegisterBeatCallback(BeatCallback&& callback) {
  beat_event_.Register(std::move(callback));
}

void Sequencer::SetNumBars(int num_bars) {
  transport_.num_bars = std::max(num_bars, 0);
  transport_.bar = (transport_.num_bars > 0)
                       ? std::min(transport_.bar, transport_.num_bars - 1)
                       : 0;
}

void Sequencer::SetNumBeats(int num_beats) {
  transport_.num_beats = std::max(num_beats, 0);
  transport_.beat = (transport_.num_beats > 0)
                        ? std::min(transport_.beat, transport_.num_beats - 1)
                        : 0;
}

void Sequencer::SetPosition(int section, int bar, int beat) {
  transport_.section = std::max(section, 0);
  transport_.bar = (transport_.num_bars > 0)
                       ? std::min(std::max(bar, 0), transport_.num_bars - 1)
                       : 0;
  transport_.beat = (transport_.num_beats > 0)
                        ? std::min(std::max(beat, 0), transport_.num_beats - 1)
                        : 0;
  leftover_samples_ = 0;
}

void Sequencer::SetTempo(float tempo) {
  transport_.tempo = std::max(tempo, 0.0f);
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
  if (leftover_samples_ == num_samples) {
    beat_event_.Trigger(transport_, 0, num_samples_per_beat_);
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
    if (leftover_samples_ > 0) {
      beat_event_.Trigger(transport_, num_samples - leftover_samples_,
                          num_samples_per_beat_);
    }
  }
}

}  // namespace barelyapi