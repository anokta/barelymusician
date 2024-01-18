#ifndef BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_

namespace barely {

/// Gain processor with a linear ramp.
class GainProcessor {
 public:
  /// Constructs new `GainProcessor`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit GainProcessor(int frame_rate) noexcept;

  /// Processes next buffer.
  ///
  /// @param buffer Buffer.
  /// @param channel_count Number of channels.
  /// @param frame_count Number of frames.
  void Process(float* buffer, int channel_count, int frame_count) noexcept;

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
  float unity_ramp_frame_count_ = 0.0f;

  // Target gain in amplitude.
  float target_gain_ = 1.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
