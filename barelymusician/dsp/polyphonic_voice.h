#ifndef BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
#define BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_

#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace barely {

/// Class template that provides polyphony of a desired voice type.
template <class VoiceType>
class PolyphonicVoice {
 public:
  /// Voice mutator callback signature.
  ///
  /// @param voice Pointer to voice.
  using VoiceCallback = std::function<void(VoiceType* voice)>;

  /// Constructs new `PolyphonicVoice` with the given `base_voice`.
  ///
  /// @param base_voice Base voice type to be used.
  /// @param max_voice_count Maximum number of voices allowed to be set.
  PolyphonicVoice(VoiceType&& base_voice, int max_voice_count) noexcept;

  /// Returns the next output sample for the given output `channel`.
  ///
  /// @param channel Output channel.
  /// @return Accumulated output sample.
  double Next(int channel) noexcept;

  /// Resizes number of available voices that can be played simultaneously.
  ///
  /// @param voice_count Number of available voices.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Resize(int voice_count) noexcept;

  /// Starts a new voice with a given pitch.
  ///
  /// @param pitch Voice pitch.
  /// @param init_voice Callback to initialize the voice for playback.
  void Start(double pitch, const VoiceCallback& init_voice = nullptr) noexcept;

  /// Stops the voice with a given pitch.
  ///
  /// @param pitch Voice pitch.
  /// @param shutdown_voice Callback to shutdown the voice.
  void Stop(double pitch, const VoiceCallback& shutdown_voice = nullptr) noexcept;

  /// Updates all the available voices with the given callback.
  ///
  /// @param update_voice Callback to update each voice.
  void Update(const VoiceCallback& update_voice) noexcept;

 private:
  // Base voice to initialize new voices.
  VoiceType base_voice_;

  // List of available voices.
  std::vector<VoiceType> voices_;

  // List of voice states, namely the voice pitch and its timestamp. Timestamp is used to determine
  // which voice to *steal* when there is no free voice available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  std::vector<std::pair<double, int>> voice_states_;
};

template <class VoiceType>
PolyphonicVoice<VoiceType>::PolyphonicVoice(VoiceType&& base_voice, int max_voice_count) noexcept
    : base_voice_(std::move(base_voice)) {
  voices_.reserve(max_voice_count);
}

template <class VoiceType>
double PolyphonicVoice<VoiceType>::Next(int channel) noexcept {
  double output = 0.0;
  for (VoiceType& voice : voices_) {
    if (voice.IsActive()) {
      output += voice.Next(channel);
    }
  }
  return output;
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Resize(int voice_count) noexcept {
  assert(voice_count >= 0);
  voices_.resize(voice_count, base_voice_);
  voice_states_.resize(voice_count, {0.0, 0});
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Start(double pitch, const VoiceCallback& init_voice) noexcept {
  const int voice_count = static_cast<int>(voices_.size());
  if (voice_count == 0) {
    // No voices available.
    return;
  }

  int voice_index = 0;
  for (int i = 0; i < voice_count; ++i) {
    if (voices_[i].IsActive()) {
      // Increment timestamp.
      ++voice_states_[i].second;
      if (voice_states_[i].second > voice_states_[voice_index].second) {
        // If no free voices available, steal the last used active voice.
        voice_index = i;
      }
    }
  }
  for (int i = 0; i < voice_count; ++i) {
    if (!voices_[i].IsActive()) {
      // Acquire a free voice.
      voice_index = i;
      break;
    }
  }
  VoiceType* voice = &voices_[voice_index];
  voice_states_[voice_index] = {pitch, 0};

  if (init_voice) {
    init_voice(voice);
  }
  voice->Start();
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Stop(double pitch, const VoiceCallback& shutdown_voice) noexcept {
  const int voice_count = static_cast<int>(voices_.size());
  for (int i = 0; i < voice_count; ++i) {
    if (voice_states_[i].first == pitch && voices_[i].IsActive()) {
      VoiceType* voice = &voices_[i];
      if (shutdown_voice) {
        shutdown_voice(voice);
      }
      voice->Stop();
    }
  }
}

template <class VoiceType>
void PolyphonicVoice<VoiceType>::Update(const VoiceCallback& update_voice) noexcept {
  update_voice(&base_voice_);
  for (VoiceType& voice : voices_) {
    update_voice(&voice);
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
