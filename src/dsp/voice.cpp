#include "dsp/voice.h"

#include "barelymusician.h"

namespace barely::internal {

Voice::Voice(int sample_rate) noexcept
    : envelope_(sample_rate), oscillator_(sample_rate), sample_player_(sample_rate) {}

bool Voice::IsActive() const noexcept { return envelope_.IsActive(); }

double Voice::Next() noexcept {
  if (sample_playback_mode_ == SamplePlaybackMode::kOnce && !sample_player_.IsActive()) {
    envelope_.Reset();
  }
  return filter_.Next(
      gain_ * envelope_.Next() *
      (oscillator_.Next() +
       ((sample_playback_mode_ != SamplePlaybackMode::kNone) ? sample_player_.Next() : 0.0)));
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  filter_.Reset();
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
