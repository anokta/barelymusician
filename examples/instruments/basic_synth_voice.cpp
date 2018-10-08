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

void BasicSynthVoice::SetEnvelopeAttack(float attack) {
  envelope_.SetAttack(attack);
}

void BasicSynthVoice::SetEnvelopeDecay(float decay) {
  envelope_.SetDecay(decay);
}

void BasicSynthVoice::SetEnvelopeSustain(float sustain) {
  envelope_.SetSustain(sustain);
}

void BasicSynthVoice::SetEnvelopeRelease(float release) {
  envelope_.SetRelease(release);
}

void BasicSynthVoice::SetOscillatorType(OscillatorType oscillator_type) {
  oscillator_.SetType(oscillator_type);
}

}  // namespace examples
}  // namespace barelyapi
