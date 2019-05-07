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
                                  voice->SetEnvelopeAttack(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeDecay,
                              kDefaultEnvelopeDecay, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeDecay(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeSustain,
                              kDefaultEnvelopeSustain, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeSustain(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentParam::kEnvelopeRelease,
                              kDefaultEnvelopeRelease, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeRelease(value);
                                });
                              });
  modulation_matrix_.Register(
      BasicSynthInstrumentParam::kOscillatorType,
      static_cast<float>(kDefaultOscillatorType), [this](float value) {
        voice_.Update([value](BasicSynthVoice* voice) {
          voice->SetOscillatorType(
              static_cast<OscillatorType>(static_cast<int>(value)));
        });
      });
}

void BasicSynthInstrument::NoteOff(float index) { voice_.Stop(index); }

void BasicSynthInstrument::NoteOn(float index, float intensity) {
  voice_.Start(index, [index, intensity](BasicSynthVoice* voice) {
    voice->SetOscillatorFrequency(FrequencyFromNoteIndex(index));
    voice->SetGain(intensity);
  });
}

void BasicSynthInstrument::Process(Frame* output) {
  const float sample = gain_ * voice_.Next();
  for (auto& output_sample : *output) {
    output_sample = sample;
  }
}

void BasicSynthInstrument::Reset() { voice_.Reset(); }

void BasicSynthInstrument::SetFloatParam(int id, float value) {
  if (!modulation_matrix_.SetParam(id, value)) {
    DLOG(WARNING) << "Failed to update float parameter with ID: " << id;
  }
}

}  // namespace examples
}  // namespace barelyapi
