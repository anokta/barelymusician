#ifndef EXAMPLES_COMMON_GAIN_PROCESSOR_H_
#define EXAMPLES_COMMON_GAIN_PROCESSOR_H_

namespace barely::examples {

/// Gain processor with a linear ramp.
class GainProcessor {
 public:
  /// Constructs new `GainProcessor`.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit GainProcessor(int sample_rate) noexcept;

  /// Processes next buffer.
  ///
  /// @param buffer Buffer.
  /// @param num_channels Number of channels.
  /// @param num_frames Number of frames.
  void Process(double* buffer, int num_channels, int num_frames) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(double gain) noexcept;

 private:
  // Current gain in amplitude.
  double gain_ = 1.0;

  // Denotes whether processor started processing or not.
  bool is_initialized_ = false;

  // Total number of ramp frames for unity gain.
  double num_unity_ramp_frames_ = 0.0;

  // Target gain in amplitude.
  double target_gain_ = 1.0;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_GAIN_PROCESSOR_H_
