#include "barelymusician/instruments/sampler_instrument.h"

#include <array>
#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelySamplerInstrument_GetDefinition() {
  return barely::SamplerInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 64;

}  // namespace

InstrumentDefinition SamplerInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{1, 0, 1},
          // Root pitch.
          ControlDefinition{0},
          // Sample player loop.
          ControlDefinition{false},
          // Attack.
          ControlDefinition{Rational(1, 20), 0, 60},
          // Decay.
          ControlDefinition{0, 0, 60},
          // Sustain.
          ControlDefinition{1, 0, 1},
          // Release.
          ControlDefinition{Rational(1, 4), 0, 60},
          // Number of voices.
          ControlDefinition{8, 1, kMaxVoiceCount},
      };
  return CustomInstrument::GetDefinition<SamplerInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
SamplerInstrument::SamplerInstrument(int frame_rate) noexcept
    : voice_(SamplerVoice(frame_rate), kMaxVoiceCount), gain_processor_(frame_rate) {}

void SamplerInstrument::Process(float* output_samples, int output_channel_count,
                                int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const float mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SamplerInstrument::SetControl(int index, Rational value,
                                   Rational /*slope_per_beat*/) noexcept {
  switch (static_cast<Control>(index)) {
    case Control::kGain:
      gain_processor_.SetGain(static_cast<float>(value));
      break;
    case Control::kRootPitch:
      root_pitch_ = value;
      break;
    case Control::kLoop:
      voice_.Update(
          [value](SamplerVoice* voice) noexcept { voice->generator().SetLoop(value > 0); });
      break;
    case Control::kAttack:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetAttack(static_cast<float>(value));
      });
      break;
    case Control::kDecay:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetDecay(static_cast<float>(value));
      });
      break;
    case Control::kSustain:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetSustain(static_cast<float>(value));
      });
      break;
    case Control::kRelease:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetRelease(static_cast<float>(value));
      });
      break;
    case Control::kVoiceCount:
      voice_.Resize(static_cast<int>(static_cast<float>(value)));
      break;
    default:
      assert(false);
      break;
  }
}

void SamplerInstrument::SetData(const void* data, int size) noexcept {
  const float* data_float = static_cast<const float*>(data);
  const float* sample_data = size > 0 ? &data_float[1] : nullptr;
  const int frame_rate = size > 0 ? static_cast<int>(data_float[0]) : 0;
  const int length = size > 0 ? size / static_cast<int>(sizeof(float)) - 1 : 0;
  voice_.Update([sample_data, frame_rate, length](SamplerVoice* voice) noexcept {
    voice->generator().SetData(sample_data, frame_rate, length);
  });
}

void SamplerInstrument::SetNoteOff(Rational pitch) noexcept { voice_.Stop(pitch); }

void SamplerInstrument::SetNoteOn(Rational pitch, Rational intensity) noexcept {
  const float speed = std::pow(2.0f, static_cast<float>(pitch - root_pitch_));
  voice_.Start(pitch, [speed, intensity](SamplerVoice* voice) noexcept {
    voice->generator().SetSpeed(speed);
    voice->set_gain(static_cast<float>(intensity));
  });
}

}  // namespace barely
