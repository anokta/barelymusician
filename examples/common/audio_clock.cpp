
#include "examples/common/audio_clock.h"

namespace barelyapi::examples {

AudioClock::AudioClock(int sample_rate)
    : sample_rate_(sample_rate), timestamp_(0.0) {}

double AudioClock::GetTimestamp() const { return timestamp_; }

void AudioClock::Update(int num_frames) {
  if (sample_rate_ > 0 && num_frames > 0) {
    timestamp_ = timestamp_ + static_cast<double>(num_frames) /
                                  static_cast<double>(sample_rate_);
  }
}

}  // namespace barelyapi::examples
