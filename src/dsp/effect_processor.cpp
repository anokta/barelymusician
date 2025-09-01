#include "dsp/effect_processor.h"

#include <cassert>

#include "dsp/control.h"

namespace barely {

namespace {

// Maximum number of delay seconds.
constexpr int kMaxDelayFrameSeconds = 10;

}  // namespace

EffectProcessor::EffectProcessor(int sample_rate, int channel_count) noexcept
    : sample_rate_(sample_rate),
      channel_count_(channel_count),
      delay_(channel_count, sample_rate * kMaxDelayFrameSeconds) {
  assert(sample_rate > 0);
  assert(channel_count > 0);
}

void EffectProcessor::Process(const float* input_samples, float* output_samples,
                              int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    delay_.Process(&input_samples[frame * channel_count_], &output_samples[frame * channel_count_],
                   current_params_.delay_mix, current_params_.delay_frame_count,
                   current_params_.delay_feedback);
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
