#include "barelymusician/base/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Sequencer::Sequencer(int sample_rate)
    : sample_rate_float_(static_cast<float>(sample_rate)),
      tempo_(0.0f),
      num_bars_per_section_(0),
      num_beats_per_bar_(0),
      num_samples_per_beat_(0),
      current_section_(0),
      current_bar_(0),
      current_beat_(0),
      leftover_samples_(0) {
  DCHECK_GE(sample_rate_float_, 0.0f);
}

void Sequencer::Reset() {
  current_section_ = 0;
  current_bar_ = 0;
  current_beat_ = 0;
  leftover_samples_ = 0;
}

int Sequencer::GetCurrentBar() const { return current_bar_; }

int Sequencer::GetCurrentBeat() const { return current_beat_; }

int Sequencer::GetCurrentSection() const { return current_section_; }

int Sequencer::GetNumSamplesPerBeat() const { return num_samples_per_beat_; }

int Sequencer::GetSampleOffset() const { return leftover_samples_; }

void Sequencer::SetNumBarsPerSection(int num_bars_per_section) {
  num_bars_per_section_ = std::max(num_bars_per_section, 0);
}

void Sequencer::SetNumBeatsPerBar(int num_beats_per_bar) {
  num_beats_per_bar_ = std::max(num_beats_per_bar, 0);
}

void Sequencer::SetTempo(float tempo) {
  tempo_ = std::max(tempo, 0.0f);
  num_samples_per_beat_ =
      (tempo_ > 0.0f)
          ? static_cast<int>(sample_rate_float_ * kSecondsFromMinutes / tempo_)
          : 0;
  if (num_samples_per_beat_ > 0) {
    // Make sure the next beat occurs after the updated |num_samples_per_beat_|
    // from the last processed beat.
    leftover_samples_ %= num_samples_per_beat_;
  }
}

void Sequencer::Update(int num_samples) {
  if (num_samples_per_beat_ == 0) {
    return;
  }
  leftover_samples_ += num_samples;
  while (leftover_samples_ >= num_samples_per_beat_) {
    // Update beat count.
    ++current_beat_;
    leftover_samples_ -= num_samples_per_beat_;
    if (num_beats_per_bar_ > 0 && current_beat_ >= num_beats_per_bar_) {
      // Update bar count.
      ++current_bar_;
      current_beat_ -= num_beats_per_bar_;
      if (num_bars_per_section_ > 0 && current_bar_ >= num_bars_per_section_) {
        // Update section count.
        ++current_section_;
        current_bar_ -= num_bars_per_section_;
      }
    }
  }
}

}  // namespace barelyapi
