
#include "examples/common/audio_clock.h"

#include <cstdint>

namespace barely::examples {

AudioClock::AudioClock(int frame_rate) noexcept : frame_rate_(frame_rate) {}

std::int64_t AudioClock::GetTimestamp() const noexcept { return timestamp_; }

void AudioClock::Update(int frame_count) noexcept {
  if (frame_rate_ > 0 && frame_count > 0) {
    timestamp_ += static_cast<std::int64_t>(frame_count);
  }
}

}  // namespace barely::examples
