#ifndef BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_

#include <barelymusician.h>

#include "delay.h"

namespace barely {

/// Effect parameters.
struct EffectParams {
  // Delay mix.
  float delay_mix = 0.0f;

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
  /// @param input_samples Array of interleaved input samples.
  /// @param output_samples Array of interleaved output samples.
  /// @param frame_count Number of frames.
  void Process(const float* input_samples, float* output_samples, int frame_count) noexcept;

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

  // Number of channels.
  int channel_count_ = 0;

  // Delay effect.
  Delay delay_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_EFFECT_PROCESSOR_H_
