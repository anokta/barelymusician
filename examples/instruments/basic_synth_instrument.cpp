#include "instruments/basic_synth_instrument.h"

namespace barelyapi {
namespace examples {

BasicSynthInstrument::BasicSynthInstrument(float sample_interval,
                                           int num_voices) {
  voices_.reserve(num_voices);
  for (int i = 0; i < num_voices; ++i) {
    voices_.emplace_back(new BasicSynthVoice(sample_interval));
  }

  InitializeModulationMatrix();
  Reset();
}

float BasicSynthInstrument::Next() {
  return gain_ * PolyphonicInstrument::Next();
}

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
      for (auto& voice : voices_) {
        voice->SetEnvelopeAttack(value);
      }
      break;
    case InstrumentFloatParam::kEnvelopeDecay:
      for (auto& voice : voices_) {
        voice->SetEnvelopeDecay(value);
      }
      break;
    case InstrumentFloatParam::kEnvelopeSustain:
      for (auto& voice : voices_) {
        voice->SetEnvelopeSustain(value);
      }
      break;
    case InstrumentFloatParam::kEnvelopeRelease:
      for (auto& voice : voices_) {
        voice->SetEnvelopeRelease(value);
      }
      break;
    case InstrumentFloatParam::kOscillatorType:
      for (auto& voice : voices_) {
        voice->SetOscillatorType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      }
      break;
    default:
      break;
  }
}

}  // namespace examples
}  // namespace barelyapi
