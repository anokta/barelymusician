#ifndef BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
#define BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_

#include <functional>
#include <vector>

#include "barelymusician/dsp/voice.h"

namespace barely {

/// Class template that provides polyphony of a desired voice type.
class PolyphonicVoice {
 public:
  /// Voice mutator callback signature.
  ///
  /// @param voice Pointer to voice.
  using VoiceCallback = std::function<void(Voice& voice)>;

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

  /// Enables retrigger.
  ///
  /// @param should_retrigger True to retrigger.
  void SetRetrigger(bool should_retrigger) noexcept;

  /// Starts a new voice with a given pitch.
  ///
  /// @param pitch Voice pitch.
  /// @param init_voice Callback to initialize the voice for playback.
  void Start(double pitch, const VoiceCallback& init_voice = nullptr) noexcept;

  /// Stops the voice with a given pitch.
  ///
  /// @param pitch Voice pitch.
  /// @param shutdown_voice Callback to shutdown the voice.
  void Stop(double pitch) noexcept;

  /// Updates all the available voices with the given callback.
  ///
  /// @param update_voice Callback to update each voice.
  void Update(const VoiceCallback& update_voice) noexcept;

 private:
  // List of voices with their pitch and timestamp. Voice timestamp is used to determine which voice
  // to steal when there are no free voices available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  struct VoiceState {
    Voice voice;
    double pitch = 0.0;
    int timestamp = 0;
  };
  std::vector<VoiceState> voice_states_;

  // Number of available voices.
  int voice_count_ = 0;

  // Determines whether to retrigger or not.
  bool should_retrigger_ = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_POLYPHONIC_VOICE_H_
