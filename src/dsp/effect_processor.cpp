#include "dsp/effect_processor.h"

#include <barelymusician.h>

#include <cassert>

#include "common/restrict.h"
#include "dsp/control.h"

namespace barely {

namespace {

// Maximum number of delay seconds.
constexpr int kMaxDelayFrameSeconds = 10;

}  // namespace

EffectProcessor::EffectProcessor(int sample_rate, int max_channel_count) noexcept
    : sample_rate_(sample_rate),
      delay_filter_(max_channel_count, sample_rate * kMaxDelayFrameSeconds) {
  assert(sample_rate > 0);
  assert(max_channel_count > 0);
}

void EffectProcessor::Process(const float* BARELY_RESTRICT delay_samples,
                              float* BARELY_RESTRICT output_samples, int channel_count,
                              int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    const int frame_offset = channel_count * frame;
    delay_filter_.Process(&delay_samples[frame_offset], &output_samples[frame_offset],
                          channel_count, current_params_.delay_mix,
                          current_params_.delay_frame_count, current_params_.delay_feedback);
    Approach();
  }
}

void EffectProcessor::SetControl(EffectControlType type, float value) noexcept {
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

void EffectProcessor::Approach() noexcept {
  ApproachValue(current_params_.delay_mix, target_params_.delay_mix);
  ApproachValue(current_params_.delay_frame_count, target_params_.delay_frame_count);
  ApproachValue(current_params_.delay_feedback, target_params_.delay_feedback);
}

}  // namespace barely
