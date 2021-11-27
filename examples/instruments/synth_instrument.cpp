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

void SynthInstrument::SetParam(int id, float value) noexcept {
  switch (static_cast<SynthInstrumentParam>(id)) {
    case SynthInstrumentParam::kGain:
      gain_ = value;
      break;
    case SynthInstrumentParam::kEnvelopeAttack:
      voice_.Update(
          [value](SynthVoice* voice) { voice->envelope().SetAttack(value); });
      break;
    case SynthInstrumentParam::kEnvelopeDecay:
      voice_.Update(
          [value](SynthVoice* voice) { voice->envelope().SetRelease(value); });
      break;
    case SynthInstrumentParam::kEnvelopeSustain:
      voice_.Update(
          [value](SynthVoice* voice) { voice->envelope().SetSustain(value); });
      break;
    case SynthInstrumentParam::kEnvelopeRelease:
      voice_.Update(
          [value](SynthVoice* voice) { voice->envelope().SetRelease(value); });
      break;
    case SynthInstrumentParam::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) {
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

ParamDefinitions SynthInstrument::GetParamDefinitions() noexcept {
  return {
      {static_cast<int>(SynthInstrumentParam::kGain), 0.25f, 0.0f, 1.0f},
      {static_cast<int>(SynthInstrumentParam::kEnvelopeAttack), 0.05f, 0.0f},
      {static_cast<int>(SynthInstrumentParam::kEnvelopeDecay), 0.0f, 0.0f},
      {static_cast<int>(SynthInstrumentParam::kEnvelopeSustain), 1.0f, 0.0f,
       1.0f},
      {static_cast<int>(SynthInstrumentParam::kEnvelopeRelease), 0.25f, 0.0f},
      {static_cast<int>(SynthInstrumentParam::kOscillatorType),
       static_cast<float>(OscillatorType::kSine)},
      {static_cast<int>(SynthInstrumentParam::kNumVoices), 8.0f, 0.0f}};
}

}  // namespace barely::examples
