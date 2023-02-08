#include "barelymusician/dsp/gain_processor.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/dsp/dsp_utils.h"

namespace barely {

namespace {

// Gain threshold of -96db in amplitude.
constexpr double kGainThreshold = 2e-5;

// Unity gain in amplitude.
constexpr double kUnityGain = 1.0;

// Total ramp duration in seconds.
constexpr double kUnityRampDurationSeconds = 0.05;

// Applies constant `gain`.
void ApplyConstantGain(double gain, double* buffer, int channel_count,
                       int frame_count) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(buffer, channel_count * frame_count, 0.0);
    return;
  }
  for (int i = 0; i < channel_count * frame_count; ++i) {
    buffer[i] *= static_cast<double>(gain);
  }
}

// Applies linear ramp of `ramp_frame_count` from `gain` to `target_gain`.
double ApplyLinearRamp(double gain, double target_gain, int ramp_frame_count,
                       double* buffer, int channel_count,
                       int frame_count) noexcept {
  const double ramp_increment_ =
      (target_gain - gain) / static_cast<double>(ramp_frame_count);
  for (int frame = 0; frame < std::min(ramp_frame_count, frame_count);
       ++frame) {
    gain += ramp_increment_;
    for (int channel = 0; channel < channel_count; ++channel) {
      buffer[channel_count * frame + channel] *= static_cast<double>(gain);
    }
  }
  return (ramp_frame_count <= frame_count) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(int frame_rate) noexcept
    : unity_ramp_frame_count_(
          FramesFromSeconds(frame_rate, kUnityRampDurationSeconds)) {}

void GainProcessor::Process(double* buffer, int channel_count,
                            int frame_count) noexcept {
  int frame = 0;
  // Apply linear ramp.
  if (gain_ != target_gain_) {
    if (is_initialized_) {
      frame = static_cast<int>(unity_ramp_frame_count_ *
                               std::abs(target_gain_ - gain_));
      if (frame > 0) {
        gain_ = ApplyLinearRamp(gain_, target_gain_, frame, buffer,
                                channel_count, frame_count);
      }
    } else {
      gain_ = target_gain_;
    }
  }
  is_initialized_ = true;
  // Apply constant gain to the rest of the buffer.
  if (frame < frame_count) {
    ApplyConstantGain(gain_, &buffer[channel_count * frame], channel_count,
                      frame_count - frame);
  }
}

void GainProcessor::SetGain(double gain) noexcept { target_gain_ = gain; }

}  // namespace barely
