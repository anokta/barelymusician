#ifndef BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_

#include "barelymusician/barelymusician.h"

namespace barely {

/// Gain processor with a linear ramp.
class GainProcessor {
 public:
  /// Constructs new `GainProcessor`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit GainProcessor(Integer frame_rate) noexcept;

  /// Processes next buffer.
  ///
  /// @param buffer Buffer.
  /// @param channel_count Number of channels.
  /// @param frame_count Number of frames.
  void Process(Real* buffer, Integer channel_count,
               Integer frame_count) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(Real gain) noexcept;

 private:
  // Current gain in amplitude.
  Real gain_ = 1.0;

  // Denotes whether processor started processing or not.
  bool is_initialized_ = false;

  // Total number of ramp frames for unity gain.
  Real unity_ramp_frame_count_ = 0.0;

  // Target gain in amplitude.
  Real target_gain_ = 1.0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_GAIN_PROCESSOR_H_
