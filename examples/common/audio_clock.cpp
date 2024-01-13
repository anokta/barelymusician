
#include "examples/common/audio_clock.h"

#include <cstdint>

#include "barelymusician/barelymusician.h"

namespace barely::examples {

AudioClock::AudioClock(int frame_rate) noexcept : frame_rate_(frame_rate) {}

Rational AudioClock::GetTimestamp() const noexcept { return Rational(tick_, frame_rate_); }

void AudioClock::Update(int frame_count) noexcept {
  if (frame_rate_ > 0 && frame_count > 0) {
    tick_ += static_cast<std::int64_t>(frame_count);
  }
}

}  // namespace barely::examples
