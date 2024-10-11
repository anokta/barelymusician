#ifndef BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
#define BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_

#include <cassert>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/dsp/voice.h"

namespace barely {

/// Class template that provides polyphony of a desired voice type.
class PolyphonicVoice {
 public:
  /// Voice mutator callback signature.
  ///
  /// @param voice Pointer to voice.
  using VoiceCallback = std::function<void(Voice* voice)>;

  /// Constructs new `PolyphonicVoice` with the given `base_voice`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param max_voice_count Maximum number of voices allowed to be set.
  PolyphonicVoice(int frame_rate, int max_voice_count) noexcept;

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
  // List of all voices.
  std::vector<Voice> voices_;

  // Number of available voices.
  int voice_count_ = 0;

  // List of voice states, namely the voice pitch and its timestamp. Timestamp is used to determine
  // which voice to *steal* when there is no free voice available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  std::vector<std::pair<double, int>> voice_states_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
