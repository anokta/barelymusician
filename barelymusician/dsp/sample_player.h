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
  double Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hertz.
  /// @param length Data length in samples.
  void SetData(const double* data, Integer frequency, Integer length) noexcept;

  /// Sets whether the playback should be looping.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop) noexcept;

  /// Sets the playback speed.
  ///
  /// @param speed Playback speed.
  void SetSpeed(double speed) noexcept;

 private:
  // Calculates per sample increment amount with the current properties.
  void CalculateIncrementPerSample() noexcept;

  // Inverse frame rate in seconds.
  double frame_interval_ = 0.0;

  // Sample data.
  const double* data_ = nullptr;

  // Sample data frame rate in hertz.
  double frequency_ = 0.0;

  // Sample data length in samples.
  double length_ = 0.0;

  // Playback speed.
  double speed_ = 1.0;

  // Playback cursor.
  double cursor_ = 0.0;

  // Increment per sample.
  double increment_ = 0.0;

  // Denotes whether the playback is looping or not.
  bool loop_ = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
