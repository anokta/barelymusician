#include "instruments/basic_synth_voice.h"

#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {
namespace examples {

BasicSynthVoice::BasicSynthVoice(float sample_interval)
    : envelope_(sample_interval), oscillator_(sample_interval), gain_(0.0f) {}

float BasicSynthVoice::Next() {
  return gain_ * envelope_.Next() * oscillator_.Next();
}

void BasicSynthVoice::Reset() {
  envelope_.Reset();
  oscillator_.Reset();
  gain_ = 0.0f;
}

bool BasicSynthVoice::IsActive() const { return envelope_.IsActive(); }

void BasicSynthVoice::Start(float index, float intensity) {
  gain_ = intensity;
  oscillator_.Reset();
  oscillator_.SetFrequency(FrequencyFromNoteIndex(index));
  envelope_.Start();
}

void BasicSynthVoice::Stop() { envelope_.Stop(); }

void BasicSynthVoice::SetFloatParam(VoiceFloatParam type, float value) {
  switch (type) {
    case VoiceFloatParam::kEnvelopeAttack:
      envelope_.SetAttack(value);
      break;
    case VoiceFloatParam::kEnvelopeDecay:
      envelope_.SetDecay(value);
      break;
    case VoiceFloatParam::kEnvelopeSustain:
      envelope_.SetSustain(value);
      break;
    case VoiceFloatParam::kEnvelopeRelease:
      envelope_.SetRelease(value);
      break;
    case VoiceFloatParam::kOscillatorFrequency:
      oscillator_.SetFrequency(value);
      break;
    case VoiceFloatParam::kOscillatorType:
      oscillator_.SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      break;
    default:
      break;
  }
}

}  // namespace examples
}  // namespace barelyapi
