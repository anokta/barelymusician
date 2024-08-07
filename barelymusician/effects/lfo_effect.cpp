#include "barelymusician/effects/lfo_effect.h"

#include <array>
#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/effects/custom_effect.h"

BarelyEffectDefinition BarelyLfoEffect_GetDefinition() {
  return barely::LfoEffect::GetDefinition();
}

namespace barely {

EffectDefinition LfoEffect::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Oscillator type.
          ControlDefinition{Control::kOscillatorType, static_cast<double>(OscillatorType::kSine),
                            0.0, static_cast<double>(OscillatorType::kNoise)},
          // Oscillator frequency.
          ControlDefinition{Control::kOscillatorFrequency, 1.0, 0.0, 32.0},
          // Intensity.
          ControlDefinition{Control::kIntensity, 1.0, 0.0, 1.0},
      };
  return CustomEffect::GetDefinition<LfoEffect>(control_definitions);
}

LfoEffect::LfoEffect(int frame_rate) noexcept : lfo_(frame_rate) { assert(frame_rate > 0); }

void LfoEffect::Process(double* output_samples, int output_channel_count,
                        int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double gain = intensity_ * lfo_.Next();
    for (int channel = 0; channel < output_channel_count; ++channel) {
      auto& sample = output_samples[output_channel_count * frame + channel];
      sample *= gain;
    }
  }
}

void LfoEffect::SetControl(int control_id, double value) noexcept {
  switch (static_cast<Control>(control_id)) {
    case Control::kOscillatorType:
      lfo_.SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      break;
    case Control::kOscillatorFrequency:
      if (value != frequency_) {
        frequency_ = value;
        lfo_.SetFrequency(value);
      }
      break;
    case Control::kIntensity:
      if (value != intensity_) {
        intensity_ = value;
        lfo_.SetFrequency(value);
      }
      break;
    default:
      assert(false);
      break;
  }
}

}  // namespace barely
