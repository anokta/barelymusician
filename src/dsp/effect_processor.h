#ifndef BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_

#include <barelymusician.h>

#include "common/restrict.h"
#include "dsp/delay_filter.h"

namespace barely {

/// Effect parameters.
struct EffectParams {
  // Delay mix.
  float delay_mix = 1.0f;

  // Number of delay frames.
  float delay_frame_count = 0.0f;

  // Delay feedback.
  float delay_feedback = 0.0f;
};

/// Class that wraps the audio processing of an effect.
class EffectProcessor {
 public:
  /// Constructs a new `EffectProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param channel_count Number of channels.
  EffectProcessor(int sample_rate, int channel_count) noexcept;

  /// Processes the next output samples.
  ///
  /// @param delay_samples Array of interleaved delay samples.
  /// @param output_samples Array of interleaved output samples.
  /// @param channel_count Number of channels.
  /// @param frame_count Number of frames.
  void Process(const float* BARELY_RESTRICT delay_samples, float* BARELY_RESTRICT output_samples,
               int channel_count, int frame_count) noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(EffectControlType type, float value) noexcept;

 private:
  // Approaches parameters.
  void Approach() noexcept;

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Delay filter.
  DelayFilter delay_filter_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_
