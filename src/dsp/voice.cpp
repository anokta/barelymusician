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
      (voice_data_.oscillator_callback(oscillator_phase_) +
       ((voice_data_.sample_playback_mode != SamplePlaybackMode::kNone)
            ? sample_player_.Next(voice_data_.sample_playback_mode == SamplePlaybackMode::kLoop)
            : 0.0));
  // Update the phasor.
  oscillator_phase_ += oscillator_increment_;
  if (oscillator_phase_ >= 1.0) {
    oscillator_phase_ -= 1.0;
  }
  return filter_.Next(output, voice_data_.filter_coefficient, voice_data_.filter_type);
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  filter_.Reset();
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
