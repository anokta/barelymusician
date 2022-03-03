#ifndef BARELYMUSICIAN_GAIN_PROCESSOR_H_
#define BARELYMUSICIAN_GAIN_PROCESSOR_H_

namespace barelyapi {

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
  void Process(float* buffer, int num_channels, int num_frames) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(float gain) noexcept;

 private:
  // Current gain in amplitude.
  float gain_ = 1.0f;

  // Denotes whether processor started processing or not.
  bool is_initialized_ = false;

  // Total number of ramp frames for unity gain.
  float num_unity_ramp_frames_ = 0.0f;

  // Target gain in amplitude.
  float target_gain_ = 1.0f;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_GAIN_PROCESSOR_H_
