#include "dsp/gain_processor.h"

#include <algorithm>
#include <cmath>

namespace barely {

namespace {

// Gain threshold of -96db in amplitude.
constexpr float kGainThreshold = 2e-5f;

// Unity gain in amplitude.
constexpr float kUnityGain = 1.0f;

// Total ramp duration in seconds.
constexpr float kUnityRampDurationSeconds = 0.05f;

// Applies constant `gain`.
void ApplyConstantGain(float gain, float* samples, int sample_count) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(samples, sample_count, 0.0f);
    return;
  }
  for (int i = 0; i < sample_count; ++i) {
    samples[i] *= gain;
  }
}

// Applies linear ramp of `ramp_sample_count` from `gain` to `target_gain`.
float ApplyLinearRamp(float gain, float target_gain, int ramp_sample_count, float* samples,
                      int sample_count) noexcept {
  const float ramp_increment_ = (target_gain - gain) / static_cast<float>(ramp_sample_count);
  for (int i = 0; i < std::min(ramp_sample_count, sample_count); ++i) {
    gain += ramp_increment_;
    samples[i] *= gain;
  }
  return (ramp_sample_count <= sample_count) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(int sample_rate) noexcept
    : unity_ramp_sample_count_(static_cast<float>(sample_rate) * kUnityRampDurationSeconds) {}

void GainProcessor::Process(float* samples, int sample_count) noexcept {
  int i = 0;
  // Apply linear ramp.
  if (gain_ != target_gain_) {
    i = static_cast<int>(unity_ramp_sample_count_ * std::abs(target_gain_ - gain_));
    gain_ = (i > 0) ? ApplyLinearRamp(gain_, target_gain_, i, samples, sample_count) : target_gain_;
  }
  // Apply constant gain to the rest of the samples.
  if (i < sample_count) {
    ApplyConstantGain(gain_, &samples[i], sample_count - i);
  }
}

void GainProcessor::SetGain(float gain) noexcept { target_gain_ = gain; }

}  // namespace barely
