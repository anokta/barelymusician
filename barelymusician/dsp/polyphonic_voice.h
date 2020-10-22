#ifndef BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
#define BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/logging.h"

namespace barelyapi {

// Class template that provides polyphony of a desired voice type.
template <class VoiceType>
class PolyphonicVoice {
 public:
  // Voice mutator callback signature.
  using VoiceCallback = std::function<void(VoiceType* voice)>;

  // Constructs new |PolyphonicVoice| with the given |base_voice|.
  //
  // @param base_voice Base voice type to be used.
  explicit PolyphonicVoice(VoiceType&& base_voice);

  // Returns the next output sample for the given output |channel|.
  //
  // @param channel Output channel.
  // @return Accumulated output sample.
  float Next(int channel);

  // Resets all voices.
  void Reset();

  // Resizes number of available voices that can be played simultaneously.
  //
  // @param num_voices Number of available voices.
  void Resize(int num_voices);

  // Starts new voice for the given |index|.
  //
  // @param index Voice (note) index.
  // @param init_voice Callback to initialize the voice for playback.
  void Start(float index, const VoiceCallback& init_voice = nullptr);

  // Stops the voice with the given |index|.
  //
  // @param index Voice (note) index.
  // @param shutdown_voice Callback to shutdown the voice.
  void Stop(float index, const VoiceCallback& shutdown_voice = nullptr);

  // Updates all the available voices with the given callback.
  //
  // @param update_voice Callback to update each voice.
  void Update(const VoiceCallback& update_voice);

 private:
  // Base voice to initialize new voices.
  VoiceType base_voice_;

  // List of available voices.
  std::vector<VoiceType> voices_;

  // List of voice states, namely the voice (note) index and its timestamp.
  // Timestamp is used to determine which voice to *steal* when there is no free
  // voice available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  std::vector<std::pair<float, int>> voice_states_;
};

template <class VoiceType>
PolyphonicVoice<VoiceType>::PolyphonicVoice(VoiceType&& base_voice)
    : base_voice_(std::move(base_voice)) {}

template <class VoiceType>
float PolyphonicVoice<VoiceType>::Next(int channel) {
  float output = 0.0f;
  for (VoiceType& voice : voices_) {
    if (voice.IsActive()) {
      output += voice.Next(channel);
    }
  }
  return output;
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Reset() {
  for (auto& voice : voices_) {
    voice.Stop();
  }
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Resize(int num_voices) {
  DCHECK_GE(num_voices, 0);
  voices_.resize(num_voices, base_voice_);
  voice_states_.resize(num_voices, {0.0f, 0});
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Start(float index,
                                       const VoiceCallback& init_voice) {
  const int num_voices = static_cast<int>(voices_.size());
  if (num_voices == 0) {
    // No voices available.
    return;
  }

  int voice_index = 0;
  for (int i = 0; i < num_voices; ++i) {
    if (voices_[i].IsActive()) {
      // Increment timestamp.
      ++voice_states_[i].second;
      if (voice_states_[i].second > voice_states_[voice_index].second) {
        // If no free voices available, steal the last used active voice.
        voice_index = i;
      }
    }
  }
  for (int i = 0; i < num_voices; ++i) {
    if (!voices_[i].IsActive()) {
      // Acquire a free voice.
      voice_index = i;
      break;
    }
  }
  VoiceType* voice = &voices_[voice_index];
  voice_states_[voice_index] = {index, 0};

  if (init_voice) {
    init_voice(voice);
  }
  voice->Start();
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Stop(float index,
                                      const VoiceCallback& shutdown_voice) {
  const int num_voices = static_cast<int>(voices_.size());
  for (int i = 0; i < num_voices; ++i) {
    if (voice_states_[i].first == index && voices_[i].IsActive()) {
      VoiceType* voice = &voices_[i];
      if (shutdown_voice) {
        shutdown_voice(voice);
      }
      voice->Stop();
    }
  }
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Update(const VoiceCallback& update_voice) {
  for (VoiceType& voice : voices_) {
    update_voice(&voice);
  }
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
