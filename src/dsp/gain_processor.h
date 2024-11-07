#ifndef BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_

namespace barely::internal {

/// Gain processor with a linear ramp.
class GainProcessor {
 public:
  /// Constructs new `GainProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit GainProcessor(int sample_rate) noexcept;

  /// Processes next buffer.
  ///
  /// @param samples Array of mono samples.
  /// @param sample_count Number of samples.
  void Process(double* samples, int sample_count) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(double gain) noexcept;

 private:
  // Current gain in amplitude.
  double gain_ = 1.0;

  // Denotes whether processor started processing or not.
  bool is_initialized_ = false;

  // Total number of ramp samples for unity gain.
  double unity_ramp_sample_count_ = 0.0;

  // Target gain in amplitude.
  double target_gain_ = 1.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
