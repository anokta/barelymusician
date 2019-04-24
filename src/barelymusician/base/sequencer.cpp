#include "barelymusician/base/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Returns the corresponding number of beats for the given |samples|.
//
// @param samples Number of samples.
// @param num_samples_per_beat Number of samples per beat.
// @return Fractional number of beats.
float BeatsFromSamples(int samples, int num_samples_per_beat) {
  return static_cast<float>(samples) / static_cast<float>(num_samples_per_beat);
}

// Returns the corresponding number of samples for the given |beats|.
//
// @param beats Fractional number of beats.
// @param num_samples_per_beat Number of samples per beat.
// @return Number of samples.
int SamplesFromBeats(float beats, int num_samples_per_beat) {
  return static_cast<int>(beats * static_cast<float>(num_samples_per_beat));
}

}  // namespace

Sequencer::Sequencer(int sample_rate)
    : num_samples_per_minute_(static_cast<float>(sample_rate) *
                              kSecondsFromMinutes),
      num_samples_per_beat_(0),
      offset_samples_(0),
      transport_({}) {
  DCHECK_GE(num_samples_per_minute_, 0.0f);
}

void Sequencer::Reset() { SetPosition(0, 0, 0); }

const Transport& Sequencer::GetTransport() const { return transport_; }

void Sequencer::RegisterBeatCallback(BeatCallback&& callback) {
  beat_event_.Register(std::move(callback));
}

void Sequencer::SetNumBars(int num_bars) {
  transport_.num_bars = std::max(num_bars, 0);
}

void Sequencer::SetNumBeats(int num_beats) {
  transport_.num_beats = std::max(num_beats, 0);
}

void Sequencer::SetPosition(int section, int bar, int beat) {
  transport_.section = std::max(section, 0);
  transport_.bar = (transport_.num_bars > 0)
                       ? std::min(std::max(bar, 0), transport_.num_bars - 1)
                       : 0;
  transport_.beat = (transport_.num_beats > 0)
                        ? std::min(std::max(beat, 0), transport_.num_beats - 1)
                        : 0;
  transport_.offset_beats = 0.0f;
  offset_samples_ = 0;
}

void Sequencer::SetTempo(float tempo) {
  transport_.tempo = std::max(tempo, 0.0f);
  num_samples_per_beat_ =
      (transport_.tempo > 0.0f)
          ? static_cast<int>(num_samples_per_minute_ / transport_.tempo)
          : 0;
  offset_samples_ =
      SamplesFromBeats(transport_.offset_beats, num_samples_per_beat_);
}

void Sequencer::Update(int num_samples) {
  if (num_samples_per_beat_ == 0) {
    return;
  }
  offset_samples_ += num_samples;
  transport_.offset_beats =
      BeatsFromSamples(offset_samples_, num_samples_per_beat_);
  if (offset_samples_ == num_samples) {
    beat_event_.Trigger(transport_, 0);
  }
  while (offset_samples_ >= num_samples_per_beat_) {
    // Update beat count.
    ++transport_.beat;
    offset_samples_ -= num_samples_per_beat_;
    transport_.offset_beats =
        BeatsFromSamples(offset_samples_, num_samples_per_beat_);
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
    if (offset_samples_ > 0) {
      beat_event_.Trigger(transport_, num_samples - offset_samples_);
    }
  }
}

}  // namespace barelyapi
