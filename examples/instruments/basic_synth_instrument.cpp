#include "examples/instruments/basic_synth_instrument.h"

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

BasicSynthInstrument::BasicSynthInstrument(int sample_rate)
    : gain_(0.0f), voice_(BasicSynthVoice(sample_rate)) {}

void BasicSynthInstrument::NoteOff(float index) { voice_.Stop(index); }

void BasicSynthInstrument::NoteOn(float index, float intensity) {
  voice_.Start(index, [index, intensity](BasicSynthVoice* voice) {
    voice->generator().SetFrequency(FrequencyFromNoteIndex(index));
    voice->set_gain(intensity);
  });
}

void BasicSynthInstrument::Process(float* output, int num_channels,
                                   int num_frames) {
  float mono_sample = 0.0f;
  for (int frame = 0; frame < num_frames; ++frame) {
    mono_sample = gain_ * voice_.Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void BasicSynthInstrument::SetParam(int id, float value) {
  switch (static_cast<BasicSynthInstrumentParam>(id)) {
    case BasicSynthInstrumentParam::kGain:
      gain_ = value;
      break;
    case BasicSynthInstrumentParam::kEnvelopeAttack:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetAttack(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeDecay:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetRelease(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeSustain:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetSustain(value);
      });
      break;
    case BasicSynthInstrumentParam::kEnvelopeRelease:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->envelope().SetRelease(value);
      });
      break;
    case BasicSynthInstrumentParam::kOscillatorType:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case BasicSynthInstrumentParam::kNumVoices:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

std::vector<ParamData> BasicSynthInstrument::GetDefaultParams() {
  return {{static_cast<int>(BasicSynthInstrumentParam::kGain), kDefaultGain},
          {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeAttack),
           kDefaultEnvelopeAttack},
          {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeDecay),
           kDefaultEnvelopeDecay},
          {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeSustain),
           kDefaultEnvelopeSustain},
          {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeRelease),
           kDefaultEnvelopeRelease},
          {static_cast<int>(BasicSynthInstrumentParam::kOscillatorType),
           static_cast<float>(kDefaultOscillatorType)},
          {static_cast<int>(BasicSynthInstrumentParam::kNumVoices),
           static_cast<float>(kDefaultNumVoices)}};
}

}  // namespace examples
}  // namespace barelyapi
