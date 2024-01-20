#include "barelymusician/instruments/synth_instrument.h"

#include <array>
#include <cassert>
#include <cstdint>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelySynthInstrument_GetDefinition() {
  return barely::SynthInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 64;

}  // namespace

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{1, 0, 1},
          // Oscillator type.
          ControlDefinition{static_cast<int64_t>(OscillatorType::kSine), 0,
                            static_cast<int64_t>(OscillatorType::kNoise)},
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
  return CustomInstrument::GetDefinition<SynthInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
SynthInstrument::SynthInstrument(int frame_rate) noexcept
    : voice_(SynthVoice(frame_rate), kMaxVoiceCount), gain_processor_(frame_rate) {}

void SynthInstrument::Process(float* output_samples, int output_channel_count,
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
void SynthInstrument::SetControl(int index, Rational value, Rational /*slope_per_frame*/) noexcept {
  switch (static_cast<Control>(index)) {
    case Control::kGain:
      gain_processor_.SetGain(static_cast<float>(value));
      break;
    case Control::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(static_cast<OscillatorType>(static_cast<int64_t>(value)));
      });
      break;
    case Control::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(static_cast<float>(value));
      });
      break;
    case Control::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetDecay(static_cast<float>(value));
      });
      break;
    case Control::kSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(static_cast<float>(value));
      });
      break;
    case Control::kRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(static_cast<float>(value));
      });
      break;
    case Control::kVoiceCount:
      voice_.Resize(static_cast<int>(static_cast<int64_t>(value)));
      break;
    default:
      assert(false);
      break;
  }
}

void SynthInstrument::SetNoteOff(Rational pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::SetNoteOn(Rational pitch, float intensity) noexcept {
  voice_.Start(pitch, [pitch, intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    voice->set_gain(intensity);
  });
}

}  // namespace barely
