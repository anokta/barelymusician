#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/dsp/voice.h"

namespace barely {

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

 private:
  Envelope envelope_;
  Oscillator oscillator_;
  SamplePlayer sample_player_;

  // Voice gain.
  double gain_ = 0.0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
