#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

namespace barely::internal {

/// Sample player that generates output samples from the provided mono data.
class SamplePlayer {
 public:
  /// Construct new `SamplePlayer` with the given sample data.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit SamplePlayer(int sample_rate) noexcept;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept;

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  [[nodiscard]] double Next() noexcept;

  /// Resets the state.
  void Reset() noexcept;

  /// Sets the sample data.
  ///
  /// @param data Sample data.
  /// @param frequency Data sampling frequency in hertz.
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
  double sample_interval_ = 0.0;

  // Sample data.
  const double* data_ = nullptr;

  // Sample data sampling rate in hertz.
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

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
