#include "dsp/engine_processor.h"

#include <barelymusician.h>

#include <cassert>

#include "dsp/decibels.h"

namespace barely {

namespace {

// Maximum number of delay seconds.
constexpr int kMaxDelayFrameSeconds = 10;

}  // namespace

EngineProcessor::EngineProcessor(int sample_rate, int max_channel_count) noexcept
    : sample_rate_(sample_rate),
      delay_filter_(max_channel_count, sample_rate * kMaxDelayFrameSeconds),
      sidechain_(sample_rate, max_channel_count),
      delay_frame_(max_channel_count, 0.0f),
      sidechain_frame_(max_channel_count, 0.0f) {
  assert(sample_rate > 0);
  assert(max_channel_count > 0);
}

void EngineProcessor::SetControl(EffectControlType type, float value) noexcept {
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

}  // namespace barely
