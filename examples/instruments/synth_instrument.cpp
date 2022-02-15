#include "examples/instruments/synth_instrument.h"

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "examples/instruments/enveloped_voice.h"

namespace barely::examples {

using ::barelyapi::GetFrequency;
using ::barelyapi::OscillatorType;

SynthInstrument::SynthInstrument(int sample_rate) noexcept
    : voice_(SynthVoice(sample_rate)) {}

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
    const float mono_sample = voice_.Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void SynthInstrument::SetParameter(int index, float value) noexcept {
  switch (static_cast<SynthInstrumentParameter>(index)) {
    case SynthInstrumentParameter::kEnvelopeAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthInstrumentParameter::kEnvelopeDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthInstrumentParameter::kEnvelopeSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SynthInstrumentParameter::kEnvelopeRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthInstrumentParameter::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthInstrumentParameter::kNumVoices:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

BarelyInstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static std::vector<BarelyParameterDefinition> parameter_definitions = {
      // Attack.
      BarelyParameterDefinition{0.05f, 0.0f, 60.0f},
      // Decay.
      BarelyParameterDefinition{0.0f, 0.0f, 60.0f},
      // Sustain.
      BarelyParameterDefinition{1.0f, 0.0f, 1.0f},
      // Release.
      BarelyParameterDefinition{0.25f, 0.0f, 60.0f},
      // Oscillator type.
      BarelyParameterDefinition{static_cast<float>(OscillatorType::kSine), 0.0f,
                                static_cast<float>(OscillatorType::kNoise)},
      // Number of voices.
      BarelyParameterDefinition{8.0f, 1.0f, 64.0f},
  };
  return GetInstrumentDefinition<SynthInstrument>(parameter_definitions);
}

}  // namespace barely::examples

extern "C" {

BarelyStatus BarelyExamples_CreateSynthInstrument(BarelyApi api,
                                                  int32_t sample_rate,
                                                  BarelyId* out_instrument_id) {
  return BarelyInstrument_Create(
      api, barely::examples::SynthInstrument::GetDefinition(), sample_rate,
      out_instrument_id);
}

}  // extern "C"
