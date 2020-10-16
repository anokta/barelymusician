#include "instruments/basic_synth_instrument.h"

#include <algorithm>
#include <memory>

#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {
namespace examples {

namespace {

// Default values.
const float kDefaultGain = 0.25f;
const float kDefaultEnvelopeAttack = 0.05f;
const float kDefaultEnvelopeDecay = 0.0f;
const float kDefaultEnvelopeSustain = 1.0f;
const float kDefaultEnvelopeRelease = 0.25f;
const OscillatorType kDefaultOscillatorType = OscillatorType::kSine;
const int kDefaultNumVoices = 8;

}  // namespace

void BasicSynthInstrument::Control(int id, float value) {
  if (!voice_.has_value()) return;
  switch (static_cast<BasicSynthInstrumentParam>(id)) {
    case BasicSynthInstrumentParam::kGain:
      gain_ = value;
      break;
    case BasicSynthInstrumentParam::kEnvelopeAttack:
      voice_->Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetAttack(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeDecay:
      voice_->Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetRelease(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeSustain:
      voice_->Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetSustain(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeRelease:
      voice_->Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetRelease(value);
      });
      break;
    case BasicSynthInstrumentParam::kOscillatorType:
      voice_->Update([value](BasicSynthVoice* voice) {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case BasicSynthInstrumentParam::kNumVoices:
      voice_->Resize(static_cast<int>(value));
      break;
  }
}

void BasicSynthInstrument::PrepareToPlay(int sample_rate) {
  voice_.emplace(BasicSynthVoice(sample_rate));
}

void BasicSynthInstrument::NoteOff(float index) {
  if (!voice_.has_value()) return;
  voice_->Stop(index);
}

void BasicSynthInstrument::NoteOn(float index, float intensity) {
  if (!voice_.has_value()) return;
  voice_->Start(index, [index, intensity](BasicSynthVoice* voice) {
    voice->generator().SetFrequency(FrequencyFromNoteIndex(index));
    voice->set_gain(intensity);
  });
}

void BasicSynthInstrument::Process(float* output, int num_channels,
                                   int num_frames) {
  if (!voice_.has_value()) return;
  float mono_sample = 0.0f;
  for (int frame = 0; frame < num_frames; ++frame) {
    mono_sample = gain_ * voice_->Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

InstrumentDefinition BasicSynthInstrument::GetDefinition() {
  InstrumentDefinition definition;
  definition.name = "BasicSynth";
  definition.param_definitions = {
      {BasicSynthInstrumentParam::kGain, "gain", "", kDefaultGain, 0.0f, 1.0f},
      {BasicSynthInstrumentParam::kEnvelopeAttack, "attack", "",
       kDefaultEnvelopeAttack, 0.0f, 60.0f},
      {BasicSynthInstrumentParam::kEnvelopeDecay, "decay", "",
       kDefaultEnvelopeDecay, 0.0f, 60.0f},
      {BasicSynthInstrumentParam::kEnvelopeSustain, "sustain", "",
       kDefaultEnvelopeSustain, 0.0f, 1.0f},
      {BasicSynthInstrumentParam::kEnvelopeRelease, "release", "",
       kDefaultEnvelopeRelease, 0.0f, 60.0f},
      {BasicSynthInstrumentParam::kOscillatorType, "osc type", "",
       static_cast<float>(kDefaultOscillatorType), 0.0f, 10.0f},
      {BasicSynthInstrumentParam::kNumVoices, "num voices", "",
       static_cast<float>(kDefaultNumVoices), 0.0f, 32.0f}};
  definition.get_instrument_fn = []() {
    return std::make_unique<BasicSynthInstrument>();
  };
  return definition;
}

}  // namespace examples
}  // namespace barelyapi
