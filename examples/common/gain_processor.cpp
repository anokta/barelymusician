#include "examples/common/gain_processor.h"

#include <algorithm>
#include <cmath>

namespace barely::examples {

namespace {

// Gain threshold of -96db in amplitude.
constexpr double kGainThreshold = 2e-5;

// Unity gain in amplitude.
constexpr double kUnityGain = 1.0;

// Total ramp duration in seconds.
constexpr double kUnityRampDurationSeconds = 0.05;

// Applies constant `gain`.
void ApplyConstantGain(double gain, double* buffer, int num_channels,
                       int num_frames) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(buffer, num_channels * num_frames, 0.0);
    return;
  }
  for (int i = 0; i < num_channels * num_frames; ++i) {
    buffer[i] *= static_cast<double>(gain);
  }
}

// Applies linear ramp of `num_ramp_frames` from `gain` to `target_gain`.
double ApplyLinearRamp(double gain, double target_gain, int num_ramp_frames,
                       double* buffer, int num_channels,
                       int num_frames) noexcept {
  const double ramp_increment_ =
      (target_gain - gain) / static_cast<double>(num_ramp_frames);
  for (int frame = 0; frame < std::min(num_ramp_frames, num_frames); ++frame) {
    gain += ramp_increment_;
    for (int channel = 0; channel < num_channels; ++channel) {
      buffer[num_channels * frame + channel] *= static_cast<double>(gain);
    }
  }
  return (num_ramp_frames <= num_frames) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(int sample_rate) noexcept
    : num_unity_ramp_frames_(static_cast<double>(sample_rate) *
                             kUnityRampDurationSeconds) {}

void GainProcessor::Process(double* buffer, int num_channels,
                            int num_frames) noexcept {
  int frame = 0;
  // Apply linear ramp.
  if (gain_ != target_gain_) {
    if (is_initialized_) {
      frame = static_cast<int>(num_unity_ramp_frames_ *
                               std::abs(target_gain_ - gain_));
      if (frame > 0) {
        gain_ = ApplyLinearRamp(gain_, target_gain_, frame, buffer,
                                num_channels, num_frames);
      }
    } else {
      gain_ = target_gain_;
    }
  }
  is_initialized_ = true;
  // Apply constant gain to the rest of the buffer.
  if (frame < num_frames) {
    ApplyConstantGain(gain_, &buffer[num_channels * frame], num_channels,
                      num_frames - frame);
  }
}

void GainProcessor::SetGain(double gain) noexcept { target_gain_ = gain; }

}  // namespace barely::examples
