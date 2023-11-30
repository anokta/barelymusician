#include "barelymusician/effects/low_pass_effect.h"

#include <cassert>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/one_pole_filter.h"
#include "barelymusician/effects/custom_effect.h"

BarelyEffectDefinition BarelyLowPassEffect_GetDefinition() {
  return barely::LowPassEffect::GetDefinition();
}

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
EffectDefinition LowPassEffect::GetDefinition() noexcept {
  static const std::vector<ControlDefinition> control_definitions = {
      // Cutoff frequency.
      ControlDefinition{48000.0, 0.0},
  };
  return CustomEffect::GetDefinition<LowPassEffect>(control_definitions);
}

LowPassEffect::LowPassEffect(int frame_rate) noexcept : frame_rate_(frame_rate) {
  assert(frame_rate > 0);
  for (auto& filter : filters_) {
    filter.SetType(FilterType::kLowPass);
  }
}

void LowPassEffect::Process(double* output_samples, int output_channel_count,
                            int output_frame_count) noexcept {
  assert(output_channel_count <= kMaxChannelCount);
  for (int frame = 0; frame < output_frame_count; ++frame) {
    for (int channel = 0; channel < output_channel_count; ++channel) {
      auto& sample = output_samples[output_channel_count * frame + channel];
      sample = filters_[channel].Next(sample);
    }
  }
}

void LowPassEffect::SetControl(int index, double value, double /*slope_per_frame*/) noexcept {
  switch (static_cast<Control>(index)) {
    case Control::kCutoffFrequency:
      for (auto& filter : filters_) {
        filter.SetCoefficient(GetFilterCoefficient(frame_rate_, value));
      }
      break;
  }
}

}  // namespace barely
