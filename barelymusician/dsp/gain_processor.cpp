#include "barelymusician/dsp/gain_processor.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Gain threshold of -96db in amplitude.
constexpr Real kGainThreshold = 2e-5;

// Unity gain in amplitude.
constexpr Real kUnityGain = 1.0;

// Total ramp duration in seconds.
constexpr Real kUnityRampDurationSeconds = 0.05;

// Applies constant `gain`.
void ApplyConstantGain(Real gain, Real* buffer, Integer channel_count,
                       Integer frame_count) noexcept {
  if (std::abs(gain - kUnityGain) < kGainThreshold) {
    return;
  }
  if (std::abs(gain) < kGainThreshold) {
    std::fill_n(buffer, channel_count * frame_count, 0.0);
    return;
  }
  for (Integer i = 0; i < channel_count * frame_count; ++i) {
    buffer[i] *= static_cast<Real>(gain);
  }
}

// Applies linear ramp of `ramp_frame_count` from `gain` to `target_gain`.
Real ApplyLinearRamp(Real gain, Real target_gain, Integer ramp_frame_count,
                     Real* buffer, Integer channel_count,
                     Integer frame_count) noexcept {
  const Real ramp_increment_ =
      (target_gain - gain) / static_cast<Real>(ramp_frame_count);
  for (Integer frame = 0; frame < std::min(ramp_frame_count, frame_count);
       ++frame) {
    gain += ramp_increment_;
    for (Integer channel = 0; channel < channel_count; ++channel) {
      buffer[channel_count * frame + channel] *= static_cast<Real>(gain);
    }
  }
  return (ramp_frame_count <= frame_count) ? target_gain : gain;
}

}  // namespace

GainProcessor::GainProcessor(Integer frame_rate) noexcept
    : unity_ramp_frame_count_(static_cast<Real>(frame_rate) *
                              kUnityRampDurationSeconds) {}

void GainProcessor::Process(Real* buffer, Integer channel_count,
                            Integer frame_count) noexcept {
  Integer frame = 0;
  // Apply linear ramp.
  if (gain_ != target_gain_) {
    if (is_initialized_) {
      frame = static_cast<Integer>(unity_ramp_frame_count_ *
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

void GainProcessor::SetGain(Real gain) noexcept { target_gain_ = gain; }

}  // namespace barely
