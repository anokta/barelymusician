#include "instruments/basic_synth_instrument.h"

#include <algorithm>

#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {
namespace examples {

namespace {

// Default values.
const float kDefaultGain = 1.0f;
const float kDefaultEnvelopeAttack = 0.05f;
const float kDefaultEnvelopeDecay = 0.0f;
const float kDefaultEnvelopeSustain = 1.0f;
const float kDefaultEnvelopeRelease = 0.25f;
const OscillatorType kDefaultOscillatorType = OscillatorType::kSine;

// Helper method to get the corresponding |ParamId| for the given |param_enum|.
ParamId ParamIdFromEnum(BasicSynthInstrumentFloatParam param_enum) {
  return static_cast<ParamId>(param_enum);
}

}  // namespace

BasicSynthInstrument::BasicSynthInstrument(float sample_interval,
                                           int num_voices)
    : voice_(BasicSynthVoice(sample_interval)) {
  voice_.Resize(num_voices);

  // Register parameters.
  modulation_matrix_.Register(
      BasicSynthInstrumentFloatParam::kGain,
      kDefaultGain / static_cast<float>(num_voices),
      [this](float value) { gain_ = std::max(0.0f, value); });
  modulation_matrix_.Register(BasicSynthInstrumentFloatParam::kEnvelopeAttack,
                              kDefaultEnvelopeAttack, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeAttack(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentFloatParam::kEnvelopeDecay,
                              kDefaultEnvelopeDecay, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeDecay(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentFloatParam::kEnvelopeSustain,
                              kDefaultEnvelopeSustain, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeSustain(value);
                                });
                              });
  modulation_matrix_.Register(BasicSynthInstrumentFloatParam::kEnvelopeRelease,
                              kDefaultEnvelopeRelease, [this](float value) {
                                voice_.Update([value](BasicSynthVoice* voice) {
                                  voice->SetEnvelopeRelease(value);
                                });
                              });
  modulation_matrix_.Register(
      BasicSynthInstrumentFloatParam::kOscillatorType,
      static_cast<float>(kDefaultOscillatorType), [this](float value) {
        voice_.Update([value](BasicSynthVoice* voice) {
          voice->SetOscillatorType(
              static_cast<OscillatorType>(static_cast<int>(value)));
        });
      });
}

float BasicSynthInstrument::Next() { return gain_ * voice_.Next(); }

void BasicSynthInstrument::Reset() {
  voice_.Reset();
  modulation_matrix_.Reset();
}

void BasicSynthInstrument::NoteOn(float index, float intensity) {
  voice_.Start(index, [index, intensity](BasicSynthVoice* voice) {
    voice->SetOscillatorFrequency(FrequencyFromNoteIndex(index));
    voice->SetGain(intensity);
  });
}

void BasicSynthInstrument::NoteOff(float index) { voice_.Stop(index); }

bool BasicSynthInstrument::SetFloatParam(ParamId id, float value) {
  return modulation_matrix_.SetParam(id, value);
}

}  // namespace examples
}  // namespace barelyapi
