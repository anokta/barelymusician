#include "barelymusician/dsp/polyphonic_voice.h"

namespace barely {

PolyphonicVoice::PolyphonicVoice(int frame_rate, int max_voice_count) noexcept
    : voices_(max_voice_count, Voice(frame_rate)) {}

double PolyphonicVoice::Next(int channel) noexcept {
  double output = 0.0;
  for (int i = 0; i < voice_count_; ++i) {
    if (voices_[i].IsActive()) {
      output += voices_[i].Next(channel);
    }
  }
  return output;
}

void PolyphonicVoice::Resize(int voice_count) noexcept {
  voice_count_ = voice_count;
  voice_states_.resize(voice_count, {0.0, 0});
}

void PolyphonicVoice::Start(double pitch, const VoiceCallback& init_voice) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }

  int voice_index = 0;
  for (int i = 0; i < voice_count_; ++i) {
    if (voices_[i].IsActive()) {
      // Increment timestamp.
      ++voice_states_[i].second;
      if (voice_states_[i].second > voice_states_[voice_index].second) {
        // If no free voices available, steal the last used active voice.
        voice_index = i;
      }
    }
  }
  for (int i = 0; i < voice_count_; ++i) {
    if (!voices_[i].IsActive()) {
      // Acquire a free voice.
      voice_index = i;
      break;
    }
  }
  Voice* voice = &voices_[voice_index];
  voice_states_[voice_index] = {pitch, 0};

  if (init_voice) {
    init_voice(voice);
  }
  voice->Start();
}

void PolyphonicVoice::Stop(double pitch, const VoiceCallback& shutdown_voice) noexcept {
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].first == pitch && voices_[i].IsActive()) {
      Voice* voice = &voices_[i];
      if (shutdown_voice) {
        shutdown_voice(voice);
      }
      voice->Stop();
    }
  }
}

void PolyphonicVoice::Update(const VoiceCallback& update_voice) noexcept {
  for (Voice& voice : voices_) {
    update_voice(&voice);
  }
}

}  // namespace barely
