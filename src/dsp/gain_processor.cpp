#include "dsp/gain_processor.h"

#include <algorithm>
#include <cmath>

namespace barely::internal {

namespace {

// Gain threshold of -96db in amplitude.
constexpr double kGainThreshold = 2e-5;

// Unity gain in amplitude.
constexpr double kUnityGain = 1.0;

// Total ramp duration in seconds.
constexpr double kUnityRampDurationSeconds = 0.05;

// Applies constant `gain`.
void ApplyConstantGain(double gain, double* samples, int sample_count) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(samples, sample_count, 0.0);
    return;
  }
  for (int i = 0; i < sample_count; ++i) {
    samples[i] *= gain;
  }
}

// Applies linear ramp of `ramp_sample_count` from `gain` to `target_gain`.
double ApplyLinearRamp(double gain, double target_gain, int ramp_sample_count, double* samples,
                       int sample_count) noexcept {
  const double ramp_increment_ = (target_gain - gain) / static_cast<double>(ramp_sample_count);
  for (int i = 0; i < std::min(ramp_sample_count, sample_count); ++i) {
    gain += ramp_increment_;
    samples[i] *= gain;
  }
  return (ramp_sample_count <= sample_count) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(int sample_rate) noexcept
    : unity_ramp_sample_count_(static_cast<double>(sample_rate) * kUnityRampDurationSeconds) {}

void GainProcessor::Process(double* samples, int sample_count) noexcept {
  int i = 0;
  // Apply linear ramp.
  if (gain_ != target_gain_) {
    if (is_initialized_) {
      i = static_cast<int>(unity_ramp_sample_count_ * std::abs(target_gain_ - gain_));
      if (i > 0) {
        gain_ = ApplyLinearRamp(gain_, target_gain_, i, samples, sample_count);
      }
    } else {
      gain_ = target_gain_;
    }
  }
  is_initialized_ = true;
  // Apply constant gain to the rest of the samples.
  if (i < sample_count) {
    ApplyConstantGain(gain_, &samples[i], sample_count - i);
  }
}

void GainProcessor::SetGain(double gain) noexcept { target_gain_ = gain; }

}  // namespace barely::internal
