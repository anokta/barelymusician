#include "barelymusician/base/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"

namespace barelyapi {

Sequencer::Sequencer(int sample_rate)
    : sample_rate_float_(static_cast<float>(sample_rate)),
      bpm_(0.0f),
      beat_length_(0.0f),
      num_samples_per_beat_(0),
      num_beats_per_bar_(0),
      num_bars_per_section_(0) {
  Reset();
}

void Sequencer::Reset() {
  sample_offset_ = 0;
  current_beat_ = 0;
  current_bar_ = 0;
  current_section_ = 0;
}

void Sequencer::SetBpm(float bpm) {
  bpm_ = std::max(bpm, 0.0f);
  CalculateNumSamplesPerBeat();
}

void Sequencer::SetNumBarsPerSection(int num_bars_per_section) {
  num_bars_per_section_ = std::max(num_bars_per_section, 0);
}

void Sequencer::SetTimeSignature(int num_beats_per_bar, NoteValue beat_length) {
  // Traditionally, unit beat length is a quarter note in musical notations.
  num_beats_per_bar_ = std::max(num_beats_per_bar, 0);
  beat_length_ = static_cast<float>(beat_length) /
                 static_cast<float>(NoteValue::kQuarterNote);
  CalculateNumSamplesPerBeat();
}

void Sequencer::CalculateNumSamplesPerBeat() {
  const float bpm_length = bpm_ * beat_length_;
  const float beat_length_seconds =
      (bpm_length > 0.0f) ? kSecondsFromMinutes / bpm_length : 0.0f;
  num_samples_per_beat_ =
      static_cast<int>(beat_length_seconds * sample_rate_float_);
  if (num_samples_per_beat_ > 0) {
    // Make sure the next beat occurs after the updated |num_samples_per_beat_|
    // from the last processed beat.
    sample_offset_ %= num_samples_per_beat_;
  }
}

void Sequencer::Update(int num_samples) {
  // Update beat count.
  if (num_samples_per_beat_ == 0) {
    return;
  }
  sample_offset_ += num_samples;
  current_beat_ += sample_offset_ / num_samples_per_beat_;
  sample_offset_ %= num_samples_per_beat_;
  // Update bar count.
  if (num_beats_per_bar_ == 0) {
    return;
  }
  current_bar_ += current_beat_ / num_beats_per_bar_;
  current_beat_ %= num_beats_per_bar_;
  // Update section count.
  if (num_bars_per_section_ == 0) {
    return;
  }
  current_section_ += current_bar_ / num_bars_per_section_;
  current_bar_ %= num_bars_per_section_;
}

}  // namespace barelyapi
