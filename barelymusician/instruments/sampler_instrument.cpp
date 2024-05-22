#include "barelymusician/instruments/sampler_instrument.h"

#include <array>
#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
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
          ControlDefinition{Control::kGain, 1.0, 0.0, 1.0},
          // Root pitch.
          ControlDefinition{Control::kRootPitch, 0.0},
          // Sample player loop.
          ControlDefinition{Control::kLoop, false},
          // Attack.
          ControlDefinition{Control::kAttack, 0.05, 0.0, 60.0},
          // Decay.
          ControlDefinition{Control::kDecay, 0.0, 0.0, 60.0},
          // Sustain.
          ControlDefinition{Control::kSustain, 0.0, 0.0, 1.0},
          // Release.
          ControlDefinition{Control::kRelease, 0.25, 0.0, 60.0},
          // Number of voices.
          ControlDefinition{Control::kVoiceCount, 8, 1, kMaxVoiceCount},
      };
  return CustomInstrument::GetDefinition<SamplerInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
SamplerInstrument::SamplerInstrument(int frame_rate) noexcept
    : voice_(SamplerVoice(frame_rate), kMaxVoiceCount), gain_processor_(frame_rate) {}

void SamplerInstrument::Process(double* output_samples, int output_channel_count,
                                int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SamplerInstrument::SetControl(int id, double value, double /*slope*/) noexcept {
  switch (static_cast<Control>(id)) {
    case Control::kGain:
      gain_processor_.SetGain(value);
      break;
    case Control::kRootPitch:
      root_pitch_ = value;
      break;
    case Control::kLoop:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->generator().SetLoop(static_cast<bool>(value));
      });
      break;
    case Control::kAttack:
      voice_.Update([value](SamplerVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      break;
    case Control::kDecay:
      voice_.Update([value](SamplerVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      break;
    case Control::kSustain:
      voice_.Update([value](SamplerVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      break;
    case Control::kRelease:
      voice_.Update([value](SamplerVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      break;
    case Control::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
    default:
      assert(false);
      break;
  }
}

void SamplerInstrument::SetData(const void* data, int size) noexcept {
  const double* data_double = static_cast<const double*>(data);
  const double* sample_data = size > 0 ? &data_double[1] : nullptr;
  const int frame_rate = size > 0 ? static_cast<int>(data_double[0]) : 0;
  const int length = size > 0 ? size / static_cast<int>(sizeof(double)) - 1 : 0;
  voice_.Update([sample_data, frame_rate, length](SamplerVoice* voice) noexcept {
    voice->generator().SetData(sample_data, frame_rate, length);
  });
}

void SamplerInstrument::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void SamplerInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  const double speed = std::pow(2.0, pitch - root_pitch_);
  voice_.Start(pitch, [speed, intensity](SamplerVoice* voice) noexcept {
    voice->generator().SetSpeed(speed);
    voice->set_gain(intensity);
  });
}

}  // namespace barely
