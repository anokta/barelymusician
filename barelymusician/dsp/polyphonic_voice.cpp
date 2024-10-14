#include "barelymusician/dsp/polyphonic_voice.h"

namespace barely {

PolyphonicVoice::PolyphonicVoice(int frame_rate, int max_voice_count) noexcept
    : voice_states_(max_voice_count, {Voice(frame_rate)}) {}

double PolyphonicVoice::Next(int channel) noexcept {
  double output = 0.0;
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].voice.IsActive()) {
      output += voice_states_[i].voice.Next(channel);
    }
  }
  return output;
}

void PolyphonicVoice::Resize(int voice_count) noexcept {
  for (int i = voice_count_; i < voice_count; ++i) {
    // Copy over the voice settings.
    voice_states_[i].voice = voice_states_[0].voice;
  }
  voice_count_ = voice_count;
}

void PolyphonicVoice::SetRetrigger(bool should_retrigger) noexcept {
  should_retrigger_ = should_retrigger;
}

void PolyphonicVoice::Start(double pitch, const VoiceCallback& init_voice) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }

  int voice_index = -1;
  int oldest_voice_index = 0;
  for (int i = 0; i < voice_count_; ++i) {
    if (should_retrigger_ && voice_states_[i].pitch == pitch) {
      // Retrigger the existing voice.
      voice_index = i;
    }

    if (voice_states_[i].voice.IsActive()) {
      // Increment timestamp.
      ++voice_states_[i].timestamp;
      if (voice_states_[i].timestamp > voice_states_[oldest_voice_index].timestamp) {
        oldest_voice_index = i;
      }
    } else if (voice_index == -1) {
      // Acquire a free voice.
      voice_index = i;
    }
  }
  if (voice_index == -1) {
    // If no voices are available to acquire, steal the oldest active voice.
    voice_index = oldest_voice_index;
  }
  VoiceState& voice_state = voice_states_[voice_index];

  voice_state.pitch = pitch;
  voice_state.timestamp = 0;
  if (init_voice) {
    init_voice(voice_state.voice);
  }
  voice_state.voice.Start();
}

void PolyphonicVoice::Stop(double pitch) noexcept {
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].pitch == pitch && voice_states_[i].voice.IsActive()) {
      voice_states_[i].voice.Stop();
    }
  }
}

void PolyphonicVoice::Update(const VoiceCallback& update_voice) noexcept {
  for (int i = 0; i < voice_count_; ++i) {
    update_voice(voice_states_[i].voice);
  }
}

}  // namespace barely
