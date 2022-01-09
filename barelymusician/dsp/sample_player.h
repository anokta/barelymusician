#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include "barelymusician/dsp/generator.h"

namespace barelyapi {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public Generator {
 public:
  /// Construct new |SamplePlayer| with the given sample data.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit SamplePlayer(int sample_rate) noexcept;

  /// Implements |Generator|.
  float Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hz.
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

  // Inverse sampling rate in seconds.
  const float sample_interval_;

  // Sample data.
  const float* data_;

  // Sample data sampling rate in hz.
  float frequency_;

  // Sample data length in samples.
  float length_;

  // Denotes whether the playback is looping or not.
  bool loop_;

  // Playback speed.
  float speed_;

  // Playback cursor.
  float cursor_;

  // Increment per sample.
  float increment_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
