#include "dsp/voice.h"

#include "barelymusician.h"

namespace barely::internal {

Voice::Voice(int sample_rate, const VoiceData& voice_data) noexcept
    : envelope_(voice_data.adsr), sample_player_(sample_rate), voice_data_(voice_data) {}

bool Voice::IsActive() const noexcept { return envelope_.IsActive(); }

double Voice::Next() noexcept {
  if (voice_data_.sample_playback_mode == SamplePlaybackMode::kOnce && !sample_player_.IsActive()) {
    envelope_.Reset();
  }
  const double output =
      gain_ * envelope_.Next() *
      (voice_data_.oscillator_callback(oscillator_increment_, oscillator_phase_) +
       ((voice_data_.sample_playback_mode != SamplePlaybackMode::kNone)
            ? sample_player_.Next(voice_data_.sample_playback_mode == SamplePlaybackMode::kLoop)
            : 0.0));
  return voice_data_.filter_callback(output, voice_data_.filter_coefficient, filter_state_);
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  filter_state_ = 0.0;
  oscillator_phase_ = 0.0;
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
