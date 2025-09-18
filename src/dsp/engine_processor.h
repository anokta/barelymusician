#ifndef BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <unordered_map>

#include "api/instrument.h"
#include "common/constants.h"
#include "dsp/control.h"
#include "dsp/decibels.h"
#include "dsp/delay_filter.h"
#include "dsp/instrument_processor.h"
#include "dsp/sidechain.h"

namespace barely {

// Maximum number of delay seconds.
inline constexpr int kMaxDelayFrameSeconds = 10;

/// Effect parameters.
struct EffectParams {
  // Delay mix.
  float delay_mix = 1.0f;

  // Number of delay frames.
  float delay_frame_count = 0.0f;

  // Delay feedback.
  float delay_feedback = 0.0f;

  // Sidechain mix.
  float sidechain_mix = 1.0f;

  // Sidechain threshold in decibels.
  float sidechain_threshold_db = 0.0f;

  // Sidechain ratio.
  float sidechain_ratio = 1.0f;
};

/// Class that wraps the audio processing of an effect.
class EngineProcessor {
 public:
  /// Constructs a new `EngineProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit EngineProcessor(int sample_rate) noexcept
      : sample_rate_(sample_rate),
        delay_filter_(sample_rate * kMaxDelayFrameSeconds),
        sidechain_(sample_rate) {
    assert(sample_rate > 0);
  }

  /// Processes the next output samples.
  ///
  /// @param instruments Set of instruments.
  /// @param output_samples Array of interleaved output samples.
  /// @param output_frame_count Number of output frames.
  void Process(
      const std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor*>& instruments,
      float* output_samples, int output_frame_count) noexcept {
    for (int frame = 0; frame < output_frame_count; ++frame) {
      delay_frame_.fill(0.0f);
      sidechain_frame_.fill(0.0f);

      float* delay_frame = delay_frame_.data();
      float* sidechain_frame = sidechain_frame_.data();
      float* output_frame = &output_samples[kStereoChannelCount * frame];

      for (const auto& [_, processor] : instruments) {
        processor->Process<true>(delay_frame, sidechain_frame, output_frame);
      }
      sidechain_.Process(sidechain_frame, current_params_.sidechain_mix,
                         current_params_.sidechain_threshold_db, current_params_.sidechain_ratio);
      for (const auto& [_, processor] : instruments) {
        processor->Process<false>(delay_frame, sidechain_frame, output_frame);
      }

      delay_filter_.Process(delay_frame, output_frame, current_params_.delay_mix,
                            current_params_.delay_frame_count, current_params_.delay_feedback);

      Approach();
    }
  }

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(EffectControlType type, float value) noexcept {
    switch (type) {
      case EffectControlType::kDelayMix:
        target_params_.delay_mix = value;
        break;
      case EffectControlType::kDelayTime:
        target_params_.delay_frame_count = value * static_cast<float>(sample_rate_);
        break;
      case EffectControlType::kDelayFeedback:
        target_params_.delay_feedback = value;
        break;
      case EffectControlType::kSidechainMix:
        target_params_.sidechain_mix = value;
        break;
      case EffectControlType::kSidechainAttack:
        sidechain_.SetAttack(value);
        break;
      case EffectControlType::kSidechainRelease:
        sidechain_.SetRelease(value);
        break;
      case EffectControlType::kSidechainThreshold:
        target_params_.sidechain_threshold_db = AmplitudeToDecibels(value);
        break;
      case EffectControlType::kSidechainRatio:
        target_params_.sidechain_ratio = value;
        break;
      default:
        assert(!"Invalid effect control type");
        return;
    }
  }

 private:
  // Approaches parameters.
  void Approach() noexcept {
    ApproachValue(current_params_.delay_mix, target_params_.delay_mix);
    ApproachValue(current_params_.delay_frame_count, target_params_.delay_frame_count);
    ApproachValue(current_params_.delay_feedback, target_params_.delay_feedback);
    ApproachValue(current_params_.sidechain_mix, target_params_.sidechain_mix);
    ApproachValue(current_params_.sidechain_threshold_db, target_params_.sidechain_threshold_db);
    ApproachValue(current_params_.sidechain_ratio, target_params_.sidechain_ratio);
  }

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Delay filter.
  DelayFilter delay_filter_;

  // Sidechain.
  Sidechain sidechain_;

  // Delay send frame.
  std::array<float, kStereoChannelCount> delay_frame_;

  // Sidechain send frame.
  std::array<float, kStereoChannelCount> sidechain_frame_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
