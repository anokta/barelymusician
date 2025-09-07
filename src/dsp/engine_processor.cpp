#include "dsp/engine_processor.h"

#include <barelymusician.h>

#include <cassert>

namespace barely {

namespace {

// Maximum number of delay seconds.
constexpr int kMaxDelayFrameSeconds = 10;

}  // namespace

EngineProcessor::EngineProcessor(int sample_rate, int max_channel_count,
                                 int max_frame_count) noexcept
    : sample_rate_(sample_rate),
      delay_filter_(max_channel_count, sample_rate * kMaxDelayFrameSeconds),
      delay_samples_(max_channel_count * max_frame_count) {
  assert(sample_rate > 0);
  assert(max_channel_count > 0);
  assert(max_frame_count > 0);
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
    default:
      assert(!"Invalid effect control type");
      return;
  }
}

}  // namespace barely
