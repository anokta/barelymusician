#ifndef BARELYMUSICIAN_PRESETS_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_PRESETS_DSP_SAMPLE_PLAYER_H_

#include "barelymusician/presets/dsp/generator.h"

namespace barelyapi {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public Generator {
 public:
  /// Construct new `SamplePlayer` with the given sample data.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit SamplePlayer(int sample_rate) noexcept;

  /// Implements `Generator`.
  double Next() noexcept override;
  void Reset() noexcept override;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hz.
  /// @param length Data length in samples.
  void SetData(const double* data, int frequency, int length) noexcept;

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

  // Inverse sampling rate in seconds.
  double sample_interval_;

  // Sample data.
  const double* data_;

  // Sample data sampling rate in hz.
  double frequency_;

  // Sample data length in samples.
  double length_;

  // Denotes whether the playback is looping or not.
  bool loop_;

  // Playback speed.
  double speed_;

  // Playback cursor.
  double cursor_;

  // Increment per sample.
  double increment_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_DSP_SAMPLE_PLAYER_H_
