#include "barelymusician/effects/high_pass_effect.h"

#include <array>
#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/one_pole_filter.h"
#include "barelymusician/effects/custom_effect.h"

BarelyEffectDefinition BarelyHighPassEffect_GetDefinition() {
  return barely::HighPassEffect::GetDefinition();
}

namespace barely {

EffectDefinition HighPassEffect::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Cutoff frequency.
          ControlDefinition{0.0, 0.0},
      };
  return CustomEffect::GetDefinition<HighPassEffect>(control_definitions);
}

HighPassEffect::HighPassEffect(int frame_rate) noexcept : frame_rate_(frame_rate) {
  assert(frame_rate > 0);
  for (auto& filter : filters_) {
    filter.SetType(FilterType::kHighPass);
  }
}

void HighPassEffect::Process(double* output_samples, int output_channel_count,
                             int output_frame_count) noexcept {
  assert(output_channel_count <= kMaxChannelCount);
  for (int frame = 0; frame < output_frame_count; ++frame) {
    if (cutoff_frequency_.second != 0.0) {
      cutoff_frequency_.first += cutoff_frequency_.second;
      for (auto& filter : filters_) {
        filter.SetCoefficient(GetFilterCoefficient(frame_rate_, cutoff_frequency_.first));
      }
    }
    for (int channel = 0; channel < output_channel_count; ++channel) {
      auto& sample = output_samples[output_channel_count * frame + channel];
      sample = filters_[channel].Next(sample);
    }
  }
}

void HighPassEffect::SetControl(int index, double value, double slope_per_frame) noexcept {
  switch (static_cast<Control>(index)) {
    case Control::kCutoffFrequency:
      if (value != cutoff_frequency_.first) {
        cutoff_frequency_.first = value;
        for (auto& filter : filters_) {
          filter.SetCoefficient(GetFilterCoefficient(frame_rate_, value));
        }
      }
      cutoff_frequency_.second = slope_per_frame;
      break;
    default:
      assert(false);
      break;
  }
}

}  // namespace barely
