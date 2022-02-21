#include "barelymusician/gain_processor.h"

#include <cmath>

namespace barelyapi {

namespace {

// Total ramp duration in seconds.
constexpr double kRampDurationSeconds = 0.1;

}  // namespace

GainProcessor::GainProcessor(int sample_rate) noexcept
    : num_unity_ramp_frames_(static_cast<int>(static_cast<double>(sample_rate) *
                                              kRampDurationSeconds)) {}

void GainProcessor::Process(float* buffer, int num_channels,
                            int num_frames) noexcept {
  is_initialized_ = true;
  int frame = 0;
  if (num_ramp_frames_ > 0) {
    for (; frame < num_ramp_frames_ && frame < num_frames; ++frame) {
      gain_ += ramp_increment_;
      for (int channel = 0; channel < num_channels; ++channel) {
        buffer[frame * num_channels + channel] *= gain_;
      }
      ++frame;
    }
    num_ramp_frames_ -= frame;
  }
  // TODO(#88): Better handle unity and zero gain cases.
  if (gain_ != 1.0f) {
    for (; frame < num_frames; ++frame) {
      for (int channel = 0; channel < num_channels; ++channel) {
        buffer[frame * num_channels + channel] *= gain_;
      }
    }
  }
}

void GainProcessor::SetGain(float gain) noexcept {
  if (is_initialized_) {
    num_ramp_frames_ = static_cast<int>(
        static_cast<float>(num_unity_ramp_frames_) * std::abs(gain - gain_));
    ramp_increment_ =
        num_ramp_frames_ > 0
            ? (gain - gain_) / static_cast<float>(num_ramp_frames_)
            : 0.0f;
  } else {
    gain_ = gain;
  }
}

}  // namespace barelyapi
