#include "examples/instruments/synth_instrument.h"

#include <vector>

#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/instruments/enveloped_voice.h"

namespace barely::examples {

SynthInstrument::SynthInstrument(int sample_rate) noexcept
    : gain_(0.0f), voice_(SynthVoice(sample_rate)) {}

void SynthInstrument::NoteOff(float pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::NoteOn(float pitch, float intensity) noexcept {
  voice_.Start(pitch, [pitch, intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    voice->set_gain(intensity);
  });
}

void SynthInstrument::Process(float* output, int num_channels,
                              int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    const float mono_sample = gain_ * voice_.Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void SynthInstrument::SetParam(int index, float value) noexcept {
  switch (static_cast<SynthInstrumentParam>(index)) {
    case SynthInstrumentParam::kGain:
      gain_ = value;
      break;
    case SynthInstrumentParam::kEnvelopeAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthInstrumentParam::kEnvelopeDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthInstrumentParam::kEnvelopeSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SynthInstrumentParam::kEnvelopeRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthInstrumentParam::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthInstrumentParam::kNumVoices:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  return GetInstrumentDefinition<SynthInstrument>(
      [](int sample_rate) { return SynthInstrument(sample_rate); });
}

std::vector<ParamDefinition> SynthInstrument::GetParamDefinitions() noexcept {
  return {// Gain.
          ParamDefinition{0.25f, 0.0f, 1.0f},
          // Attack.
          ParamDefinition{0.05f, 0.0f},
          // Decay.
          ParamDefinition{0.0f, 0.0f},
          // Sustain.
          ParamDefinition{1.0f, 0.0f, 1.0f},
          // Release.
          ParamDefinition{0.25f, 0.0f},
          // Oscillator type.
          ParamDefinition{static_cast<int>(OscillatorType::kSine)},
          // Number of voices.
          ParamDefinition{8, 0}};
}

}  // namespace barely::examples
