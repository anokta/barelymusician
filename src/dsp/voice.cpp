#include "dsp/voice.h"

#include "barelymusician.h"

namespace barely::internal {

Voice::Voice(int sample_rate, const VoiceData& voice_data) noexcept
    : envelope_(voice_data.adsr),
      oscillator_(sample_rate),
      sample_player_(sample_rate),
      voice_data_(voice_data) {}

bool Voice::IsActive() const noexcept { return envelope_.IsActive(); }

double Voice::Next() noexcept {
  if (voice_data_.sample_playback_mode == SamplePlaybackMode::kOnce && !sample_player_.IsActive()) {
    envelope_.Reset();
  }
  const double output =
      gain_ * envelope_.Next() *
      (oscillator_.Next(voice_data_.oscillator_shape) +
       ((voice_data_.sample_playback_mode != SamplePlaybackMode::kNone)
            ? sample_player_.Next(voice_data_.sample_playback_mode == SamplePlaybackMode::kLoop)
            : 0.0));
  return filter_.Next(output, voice_data_.filter_coefficient, voice_data_.filter_type);
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  filter_.Reset();
  oscillator_.Reset();
  sample_player_.Reset();
  envelope_.Start();
}

void Voice::Stop() noexcept {
  if (voice_data_.sample_playback_mode != SamplePlaybackMode::kOnce) {
    envelope_.Stop();
  } else if (!sample_player_.IsActive()) {
    envelope_.Reset();
  }
}

}  // namespace barely::internal
