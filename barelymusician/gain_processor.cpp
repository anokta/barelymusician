#include "barelymusician/gain_processor.h"

#include <algorithm>
#include <cmath>

namespace barelyapi {

namespace {

// Gain threshold of -96db in amplitude.
constexpr float kGainThreshold = 2e-5f;

// Unity gain in amplitude.
constexpr float kUnityGain = 1.0f;

// Total ramp duration in seconds.
constexpr float kUnityRampDurationSeconds = 0.05f;

// Applies constant `gain`.
void ApplyConstantGain(float gain, float* buffer, int num_channels,
                       int num_frames) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(buffer, num_channels * num_frames, 0.0f);
    return;
  }
  for (int i = 0; i < num_channels * num_frames; ++i) {
    buffer[i] *= gain;
  }
}

// Applies linear ramp of `num_ramp_frames` from `gain` to `target_gain`.
float ApplyLinearRamp(float gain, float target_gain, int num_ramp_frames,
                      float* buffer, int num_channels,
                      int num_frames) noexcept {
  const float ramp_increment_ =
      (target_gain - gain) / static_cast<float>(num_ramp_frames);
  for (int frame = 0; frame < std::min(num_ramp_frames, num_frames); ++frame) {
    gain += ramp_increment_;
    for (int channel = 0; channel < num_channels; ++channel) {
      buffer[num_channels * frame + channel] *= gain;
    }
  }
  return (num_ramp_frames <= num_frames) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(int sample_rate) noexcept
    : num_unity_ramp_frames_(static_cast<float>(sample_rate) *
                             kUnityRampDurationSeconds) {}

void GainProcessor::Process(float* buffer, int num_channels,
                            int num_frames) noexcept {
  int frame = 0;
  // Apply linear ramp.
  if (const float target_gain = target_gain_; gain_ != target_gain) {
    if (is_initialized_) {
      frame = static_cast<int>(num_unity_ramp_frames_ *
                               std::abs(target_gain - gain_));
      if (frame > 0) {
        gain_ = ApplyLinearRamp(gain_, target_gain, frame, buffer, num_channels,
                                num_frames);
      }
    } else {
      gain_ = target_gain;
    }
  }
  is_initialized_ = true;
  // Apply constant gain to the rest of the buffer.
  if (frame < num_frames) {
    ApplyConstantGain(gain_, &buffer[num_channels * frame], num_channels,
                      num_frames - frame);
  }
}

void GainProcessor::SetGain(float gain) noexcept { target_gain_ = gain; }

}  // namespace barelyapi
