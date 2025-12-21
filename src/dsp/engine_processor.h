#ifndef BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <unordered_map>

#include "api/instrument.h"
#include "common/constants.h"
#include "dsp/compressor.h"
#include "dsp/control.h"
#include "dsp/decibels.h"
#include "dsp/delay_filter.h"
#include "dsp/one_pole_filter.h"
#include "dsp/sidechain.h"
#include "dsp/voice_pool.h"

namespace barely {

// Maximum number of delay seconds.
inline constexpr int kMaxDelayFrameSeconds = 10;

/// Effect parameters.
struct EffectParams {
  /// Compressor parameters.
  CompressorParams compressor_params = {};

  /// Delay parameters.
  DelayParams delay_params = {};

  /// Sidechain mix.
  float sidechain_mix = 1.0f;

  /// Sidechain threshold in decibels.
  float sidechain_threshold_db = 0.0f;

  /// Sidechain ratio.
  float sidechain_ratio = 1.0f;
};

/// Class that wraps the audio processing of an effect.
class EngineProcessor {
 public:
  /// Constructs a new `EngineProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit EngineProcessor(int sample_rate) noexcept
      : compressor_(sample_rate),
        delay_filter_(sample_rate * kMaxDelayFrameSeconds),
        sidechain_(sample_rate),
        sample_rate_(sample_rate) {
    assert(sample_rate > 0);
  }

  /// Processes the next output samples.
  ///
  /// @param rng Random number generator.
  /// @param instrument_pool Instrument pool.
  /// @param voice_pool Voice pool.
  /// @param output_samples Array of interleaved output samples.
  /// @param output_frame_count Number of output frames.
  void Process(AudioRng& rng, InstrumentPool& instrument_pool, VoicePool& voice_pool,
               float* output_samples, int output_frame_count) noexcept {
    for (int frame = 0; frame < output_frame_count; ++frame) {
      float delay_frame[kStereoChannelCount] = {};
      float sidechain_frame[kStereoChannelCount] = {};
      float* output_frame = &output_samples[kStereoChannelCount * frame];

      voice_pool.Process<true>(rng, instrument_pool, delay_frame, sidechain_frame, output_frame);
      sidechain_.Process(sidechain_frame, current_params_.sidechain_mix,
                         current_params_.sidechain_threshold_db, current_params_.sidechain_ratio);
      voice_pool.Process<false>(rng, instrument_pool, delay_frame, sidechain_frame, output_frame);

      delay_filter_.Process(delay_frame, output_frame, current_params_.delay_params);

      compressor_.Process(output_frame, current_params_.compressor_params);

      Approach();
    }
  }

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(EngineControlType type, float value) noexcept {
    switch (type) {
      case EngineControlType::kCompressorMix:
        target_params_.compressor_params.mix = value;
        break;
      case EngineControlType::kCompressorAttack:
        compressor_.SetAttack(value);
        break;
      case EngineControlType::kCompressorRelease:
        compressor_.SetRelease(value);
        break;
      case EngineControlType::kCompressorThreshold:
        target_params_.compressor_params.threshold_db = AmplitudeToDecibels(value);
        break;
      case EngineControlType::kCompressorRatio:
        target_params_.compressor_params.ratio = value;
        break;
      case EngineControlType::kDelayMix:
        target_params_.delay_params.mix = value;
        break;
      case EngineControlType::kDelayTime:
        target_params_.delay_params.frame_count = value * static_cast<float>(sample_rate_);
        break;
      case EngineControlType::kDelayFeedback:
        target_params_.delay_params.feedback = value;
        break;
      case EngineControlType::kDelayLowPassFrequency:
        target_params_.delay_params.low_pass_coeff = GetFilterCoefficient(sample_rate_, value);
        break;
      case EngineControlType::kDelayHighPassFrequency:
        target_params_.delay_params.high_pass_coeff = GetFilterCoefficient(sample_rate_, value);
        break;
      case EngineControlType::kSidechainMix:
        target_params_.sidechain_mix = value;
        break;
      case EngineControlType::kSidechainAttack:
        sidechain_.SetAttack(value);
        break;
      case EngineControlType::kSidechainRelease:
        sidechain_.SetRelease(value);
        break;
      case EngineControlType::kSidechainThreshold:
        target_params_.sidechain_threshold_db = AmplitudeToDecibels(value);
        break;
      case EngineControlType::kSidechainRatio:
        target_params_.sidechain_ratio = value;
        break;
      default:
        assert(!"Invalid engine control type");
        return;
    }
  }

 private:
  // Approaches parameters.
  void Approach() noexcept {
    current_params_.compressor_params.Approach(target_params_.compressor_params);
    current_params_.delay_params.Approach(target_params_.delay_params);
    ApproachValue(current_params_.sidechain_mix, target_params_.sidechain_mix);
    ApproachValue(current_params_.sidechain_threshold_db, target_params_.sidechain_threshold_db);
    ApproachValue(current_params_.sidechain_ratio, target_params_.sidechain_ratio);
  }

  // Compressor.
  Compressor compressor_;

  // Delay filter.
  DelayFilter delay_filter_;

  // Sidechain.
  Sidechain sidechain_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};

  // Sampling rate in hertz.
  int sample_rate_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENGINE_PROCESSOR_H_
