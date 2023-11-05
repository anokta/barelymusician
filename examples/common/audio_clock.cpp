
#include "examples/common/audio_clock.h"

namespace barely::examples {

AudioClock::AudioClock(int frame_rate) noexcept : frame_rate_(frame_rate), timestamp_(0.0) {}

double AudioClock::GetTimestamp() const noexcept { return timestamp_; }

void AudioClock::Update(int frame_count) noexcept {
  if (frame_rate_ > 0 && frame_count > 0) {
    timestamp_ = timestamp_ + static_cast<double>(frame_count) / static_cast<double>(frame_rate_);
  }
}

}  // namespace barely::examples
