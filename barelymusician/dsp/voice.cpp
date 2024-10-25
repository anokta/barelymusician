#include "barelymusician/dsp/voice.h"

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Voice::Voice(int frame_rate) noexcept
    : envelope_(frame_rate), oscillator_(frame_rate), sample_player_(frame_rate) {}

bool Voice::IsActive() const noexcept { return envelope_.IsActive(); }

double Voice::Next() noexcept {
  if (sample_playback_mode_ == SamplePlaybackMode::kOnce && !sample_player_.IsActive()) {
    envelope_.Reset();
  }
  return gain_ * envelope_.Next() *
         (oscillator_.Next() +
          ((sample_playback_mode_ != SamplePlaybackMode::kNone) ? sample_player_.Next() : 0.0));
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  oscillator_.Reset();
  sample_player_.Reset();
  envelope_.Start();
}

void Voice::Stop() noexcept {
  if (sample_playback_mode_ != SamplePlaybackMode::kOnce) {
    envelope_.Stop();
  } else if (!sample_player_.IsActive()) {
    envelope_.Reset();
  }
}

}  // namespace barely::internal
