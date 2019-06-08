#include "instruments/basic_synth_instrument.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/instrument/instrument_utils.h"

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

}  // namespace

BasicSynthInstrument::BasicSynthInstrument(float sample_interval,
                                           int num_voices)
    : gain_(0.0f), voice_(BasicSynthVoice(sample_interval)) {
  voice_.Resize(num_voices);

  // Register parameters.
  modulation_matrix_.Register(
      BasicSynthInstrumentParam::kGain, kDefaultGain,
      [this](float value) { gain_ = std::max(0.0f, value); });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeAttack,
                              kDefaultEnvelopeAttack, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->envelope().SetAttack(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeDecay,
                              kDefaultEnvelopeDecay, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->envelope().SetDecay(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeSustain,
                              kDefaultEnvelopeSustain, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->envelope().SetSustain(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeRelease,
                              kDefaultEnvelopeRelease, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->envelope().SetRelease(value);
                                });
                              });
  modulation_matrix_.Register(
      BasicSynthInstrumentParam::kOscillatorType,
      static_cast<float>(kDefaultOscillatorType), [this](float value) {
        voice_.Update([value](BasicSynthVoice* voice) {
          voice->generator().SetType(
              static_cast<OscillatorType>(static_cast<int>(value)));
        });
      });
}

void BasicSynthInstrument::Clear() { voice_.Clear(); }

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

void BasicSynthInstrument::SetFloatParam(int id, float value) {
  if (!modulation_matrix_.SetParam(id, value)) {
    DLOG(WARNING) << "Failed to update float parameter with ID: " << id;
  }
}

}  // namespace examples
}  // namespace barelyapi
