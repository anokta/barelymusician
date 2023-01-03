#include "barelymusician/instruments/synth_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/enveloped_voice.h"

namespace barely {

SynthInstrument::SynthInstrument(int sample_rate) noexcept
    : voice_(SynthVoice(sample_rate)) {}

void SynthInstrument::Process(double* output_samples, int channel_count,
                              int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < channel_count; ++channel) {
      output_samples[channel_count * frame + channel] = mono_sample;
    }
  }
}

void SynthInstrument::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  voice_.Start(pitch, [pitch, intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    voice->set_gain(intensity);
  });
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SynthInstrument::SetParameter(int index, double value,
                                   double /*slope*/) noexcept {
  switch (static_cast<SynthParameter>(index)) {
    case SynthParameter::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthParameter::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthParameter::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SynthParameter::kSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SynthParameter::kRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthParameter::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static const std::vector<ParameterDefinition> parameter_definitions = {
      // Oscillator type.
      ParameterDefinition{static_cast<double>(OscillatorType::kSine), 0.0,
                          static_cast<double>(OscillatorType::kNoise)},
      // Attack.
      ParameterDefinition{0.05, 0.0, 60.0},
      // Decay.
      ParameterDefinition{0.0, 0.0, 60.0},
      // Sustain.
      ParameterDefinition{1.0, 0.0, 1.0},
      // Release.
      ParameterDefinition{0.25, 0.0, 60.0},
      // Number of voices.
      ParameterDefinition{8, 1, 64},
  };
  return GetInstrumentDefinition<SynthInstrument>(parameter_definitions);
}

}  // namespace barely
