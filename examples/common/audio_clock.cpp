
#include "examples/common/audio_clock.h"

namespace barely::examples {

AudioClock::AudioClock(Integer frame_rate) noexcept
    : frame_rate_(frame_rate), timestamp_(0) {}

Integer AudioClock::GetTimestamp() const noexcept { return timestamp_; }

void AudioClock::Update(Integer frame_count) noexcept {
  if (frame_rate_ > 0 && frame_count > 0) {
    timestamp_ += 1'000'000 * frame_count / frame_rate_;
  }
}

}  // namespace barely::examples
