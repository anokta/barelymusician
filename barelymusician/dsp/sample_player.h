#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include "barelymusician/dsp/generator.h"

namespace barely {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public Generator {
 public:
  /// Construct new `SamplePlayer` with the given sample data.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit SamplePlayer(int frame_rate) noexcept;

  /// Implements `Generator`.
  float Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hertz.
  /// @param length Data length in samples.
  void SetData(const float* data, int frequency, int length) noexcept;

  /// Sets whether the playback should be looping.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop) noexcept;

  /// Sets the playback speed.
  ///
  /// @param speed Playback speed.
  void SetSpeed(float speed) noexcept;

 private:
  // Calculates per sample increment amount with the current properties.
  void CalculateIncrementPerSample() noexcept;

  // Inverse frame rate in seconds.
  float frame_interval_ = 0.0f;

  // Sample data.
  const float* data_ = nullptr;

  // Sample data frame rate in hertz.
  float frequency_ = 0.0f;

  // Sample data length in samples.
  float length_ = 0.0f;

  // Playback speed.
  float speed_ = 1.0f;

  // Playback cursor.
  float cursor_ = 0.0f;

  // Increment per sample.
  float increment_ = 0.0f;

  // Denotes whether the playback is looping or not.
  bool loop_ = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
