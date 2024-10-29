#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include "barelymusician.h"
#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/dsp/voice.h"

namespace barely::internal {

/// Class that wraps an instrument voice.
class Voice {
 public:
  /// Constructs a new `Voice` with the given `frame_rate`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Voice(int frame_rate) noexcept;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Returns the next output sample.
  ///
  /// @return Output sample.
  double Next() noexcept;

  /// Resets the voice.
  void Reset() noexcept;

  /// Starts the voice.
  void Start() noexcept;

  /// Stops the voice.
  void Stop() noexcept;

  /// Inline getter/setter functions.
  [[nodiscard]] const Envelope& envelope() const noexcept { return envelope_; }
  [[nodiscard]] Envelope& envelope() noexcept { return envelope_; }

  [[nodiscard]] const Oscillator& oscillator() const noexcept { return oscillator_; }
  [[nodiscard]] Oscillator& oscillator() noexcept { return oscillator_; }

  [[nodiscard]] const SamplePlayer& sample_player() const noexcept { return sample_player_; }
  [[nodiscard]] SamplePlayer& sample_player() noexcept { return sample_player_; }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

  void set_sample_playback_mode(SamplePlaybackMode sample_playback_mode) noexcept {
    sample_playback_mode_ = sample_playback_mode;
    sample_player_.SetLoop(sample_playback_mode_ == SamplePlaybackMode::kLoop);
  }

 private:
  Envelope envelope_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;

  double gain_ = 0.0;
  SamplePlaybackMode sample_playback_mode_ = SamplePlaybackMode::kNone;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
