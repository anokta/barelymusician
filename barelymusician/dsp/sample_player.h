#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include "barelymusician/dsp/generator.h"

namespace barely {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public Generator {
 public:
  /// Construct new |SamplePlayer| with the given sample data.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit SamplePlayer(int sample_rate);

  /// Implements |Generator|.
  float Next() override;
  void Reset() override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in Hz.
  /// @param length Data length in samples.
  void SetData(const float* data, int frequency, int length);

  /// Sets whether the playback should be looping.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop);

  /// Sets the playback speed.
  ///
  /// @param speed Playback speed.
  void SetSpeed(float speed);

 private:
  // Calculates per sample increment amount with the current properties.
  void CalculateIncrementPerSample();

  // Inverse sampling rate in seconds.
  const float sample_interval_;

  // Sample data.
  const float* data_;

  // Sample data sampling rate in Hz.
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

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
