#include "instruments/basic_synth_instrument.h"
#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {
namespace examples {

BasicSynthInstrument::BasicSynthInstrument(float sample_interval,
                                           int num_voices)
    : voice_(BasicSynthVoice(sample_interval)) {
  voice_.Resize(num_voices);

  InitializeModulationMatrix();
}

float BasicSynthInstrument::Next() { return gain_ * voice_.Next(); }

void BasicSynthInstrument::Reset() { voice_.Reset(); }

void BasicSynthInstrument::NoteOn(float index, float intensity) {
  voice_.Start(index, [index, intensity](BasicSynthVoice* voice) {
    voice->SetOscillatorFrequency(FrequencyFromNoteIndex(index));
    voice->SetGain(intensity);
  });
}

void BasicSynthInstrument::NoteOff(float index) { voice_.Stop(index, nullptr); }

float BasicSynthInstrument::GetFloatParam(int id) const {
  const auto param = static_cast<InstrumentFloatParam>(id);
  const auto& param_it = modulation_matrix_.find(param);
  if (param_it == modulation_matrix_.end()) {
    // Not found.
    return 0.0f;
  }

  return param_it->second;
}

void BasicSynthInstrument::SetFloatParam(int id, float value) {
  const auto param = static_cast<InstrumentFloatParam>(id);
  const auto& param_it = modulation_matrix_.find(param);
  if (param_it == modulation_matrix_.end()) {
    return;
  }

  if (param_it->second != value) {
    param_it->second = value;
    UpdateParam(param_it->first, value);
  }
}

void BasicSynthInstrument::InitializeModulationMatrix() {
  modulation_matrix_[InstrumentFloatParam::kGain] = 1.0f;
  modulation_matrix_[InstrumentFloatParam::kEnvelopeAttack] = 0.05f;
  modulation_matrix_[InstrumentFloatParam::kEnvelopeDecay] = 0.0f;
  modulation_matrix_[InstrumentFloatParam::kEnvelopeSustain] = 1.0f;
  modulation_matrix_[InstrumentFloatParam::kEnvelopeRelease] = 0.25f;
  modulation_matrix_[InstrumentFloatParam::kOscillatorType] =
      static_cast<float>(OscillatorType::kSine);
  for (auto& param_it : modulation_matrix_) {
    UpdateParam(param_it.first, param_it.second);
  }
}

void BasicSynthInstrument::UpdateParam(InstrumentFloatParam param,
                                       float value) {
  switch (param) {
    case InstrumentFloatParam::kGain:
      gain_ = value;
      break;
    case InstrumentFloatParam::kEnvelopeAttack:
      voice_.Update(
          [value](BasicSynthVoice* voice) { voice->SetEnvelopeAttack(value); });
      break;
    case InstrumentFloatParam::kEnvelopeDecay:
      voice_.Update(
          [value](BasicSynthVoice* voice) { voice->SetEnvelopeDecay(value); });
      break;
    case InstrumentFloatParam::kEnvelopeSustain:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->SetEnvelopeSustain(value);
      });
      break;
    case InstrumentFloatParam::kEnvelopeRelease:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->SetEnvelopeRelease(value);
      });
      break;
    case InstrumentFloatParam::kOscillatorType:
      voice_.Update([value](BasicSynthVoice* voice) {
        voice->SetOscillatorType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    default:
      break;
  }
}

}  // namespace examples
}  // namespace barelyapi
