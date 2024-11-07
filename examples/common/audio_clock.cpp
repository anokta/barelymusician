
#include "common/audio_clock.h"

namespace barely::examples {

AudioClock::AudioClock(int sample_rate) noexcept : sample_rate_(sample_rate), timestamp_(0.0) {}

double AudioClock::GetTimestamp() const noexcept { return timestamp_; }

void AudioClock::Update(int sample_count) noexcept {
  if (sample_rate_ > 0 && sample_count > 0) {
    timestamp_ = timestamp_ + static_cast<double>(sample_count) / static_cast<double>(sample_rate_);
  }
}

}  // namespace barely::examples
