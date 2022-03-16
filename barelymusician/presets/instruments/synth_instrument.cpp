#include "barelymusician/presets/instruments/synth_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/presets/dsp/dsp_utils.h"
#include "barelymusician/presets/dsp/oscillator.h"
#include "barelymusician/presets/instruments/enveloped_voice.h"

namespace barelyapi {

using ::barely::InstrumentDefinition;
using ::barely::ParameterDefinition;
using ::barely::presets::OscillatorType;
using ::barely::presets::SynthParameter;

SynthInstrument::SynthInstrument(int sample_rate) noexcept
    : voice_(SynthVoice(sample_rate)) {}

void SynthInstrument::Process(double* output, int num_channels,
                              int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
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

void SynthInstrument::SetParameter(int index, double value,
                                   double /*slope*/) noexcept {
  switch (static_cast<SynthParameter>(index)) {
    case SynthParameter::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthParameter::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
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
    case SynthParameter::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthParameter::kNumVoices:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static std::vector<ParameterDefinition> parameter_definitions = {
      // Attack.
      ParameterDefinition{0.05, 0.0, 60.0},
      // Decay.
      ParameterDefinition{0.0, 0.0, 60.0},
      // Sustain.
      ParameterDefinition{1.0, 0.0, 1.0},
      // Release.
      ParameterDefinition{0.25, 0.0, 60.0},
      // Oscillator type.
      ParameterDefinition{static_cast<double>(OscillatorType::kSine), 0.0,
                          static_cast<double>(OscillatorType::kNoise)},
      // Number of voices.
      ParameterDefinition{8, 1, 64},
  };
  return GetInstrumentDefinition<SynthInstrument>(parameter_definitions);
}

}  // namespace barelyapi
