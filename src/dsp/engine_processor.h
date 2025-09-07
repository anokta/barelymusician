#ifndef BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_

#include <barelymusician.h>

#include <unordered_set>

#include "api/instrument.h"
#include "common/restrict.h"
#include "dsp/control.h"
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
class EngineProcessor {
 public:
  /// Constructs a new `EngineProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_channel_count Maximum number of channels.
  /// @param max_frame_count Maximum number of frames.
  EngineProcessor(int sample_rate, int max_channel_count, int max_frame_count) noexcept;

  /// Processes the next output samples.
  ///
  ///
  /// @param instruments Set of instruments.
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  void Process(const std::unordered_set<BarelyInstrument*>& instruments, float* output_samples,
               int output_channel_count, int output_frame_count) noexcept {
    std::fill_n(delay_samples_.begin(), output_channel_count * output_frame_count, 0.0f);
    for (int frame = 0; frame < output_frame_count; ++frame) {
      const int frame_offset = output_channel_count * frame;
      float* delay_frame = &delay_samples_[frame_offset];
      float* output_frame = &output_samples[frame_offset];
      for (BarelyInstrument* instrument : instruments) {
        instrument->processor().Process(delay_frame, output_frame);
      }
      delay_filter_.Process(delay_frame, output_frame, output_channel_count,
                            current_params_.delay_mix, current_params_.delay_frame_count,
                            current_params_.delay_feedback);
      Approach();
    }
  }

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(EffectControlType type, float value) noexcept;

 private:
  // Approaches parameters.
  void Approach() noexcept {
    ApproachValue(current_params_.delay_mix, target_params_.delay_mix);
    ApproachValue(current_params_.delay_frame_count, target_params_.delay_frame_count);
    ApproachValue(current_params_.delay_feedback, target_params_.delay_feedback);
  }

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Delay filter.
  DelayFilter delay_filter_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};

  // Delay samples.
  std::vector<float> delay_samples_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
