#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/generator.h"

namespace barely {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public Generator {
 public:
  /// Construct new `SamplePlayer` with the given sample data.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit SamplePlayer(Integer frame_rate) noexcept;

  /// Implements `Generator`.
  Real Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hertz.
  /// @param length Data length in samples.
  void SetData(const Real* data, Integer frequency, Integer length) noexcept;

  /// Sets whether the playback should be looping.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop) noexcept;

  /// Sets the playback speed.
  ///
  /// @param speed Playback speed.
  void SetSpeed(Real speed) noexcept;

 private:
  // Calculates per sample increment amount with the current properties.
  void CalculateIncrementPerSample() noexcept;

  // Inverse frame rate in seconds.
  Real frame_interval_ = 0.0;

  // Sample data.
  const Real* data_ = nullptr;

  // Sample data frame rate in hertz.
  Real frequency_ = 0.0;

  // Sample data length in samples.
  Real length_ = 0.0;

  // Playback speed.
  Real speed_ = 1.0;

  // Playback cursor.
  Real cursor_ = 0.0;

  // Increment per sample.
  Real increment_ = 0.0;

  // Denotes whether the playback is looping or not.
  bool loop_ = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
