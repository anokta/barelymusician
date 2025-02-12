#ifndef BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_

namespace barely {

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
  void Process(float* samples, int sample_count) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(float gain) noexcept;

 private:
  // Current gain in amplitude.
  float gain_ = 0.0f;

  // Total number of ramp samples for unity gain.
  float unity_ramp_sample_count_ = 0.0f;

  // Target gain in amplitude.
  float target_gain_ = 1.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
