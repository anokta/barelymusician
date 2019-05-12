#include "instruments/basic_sampler_voice.h"

namespace barelyapi {
namespace examples {

BasicSamplerVoice::BasicSamplerVoice(float sample_interval)
    : envelope_(sample_interval),
      sample_player_(sample_interval),
      gain_(0.0f) {}

float BasicSamplerVoice::Next() {
  return gain_ * envelope_.Next() * sample_player_.Next();
}

void BasicSamplerVoice::Reset() {
  envelope_.Reset();
  sample_player_.Reset();
  gain_ = 0.0f;
}

bool BasicSamplerVoice::IsActive() const { return envelope_.IsActive(); }

void BasicSamplerVoice::Start() {
  sample_player_.Reset();
  envelope_.Start();
}

void BasicSamplerVoice::Stop() { envelope_.Stop(); }

void BasicSamplerVoice::SetEnvelopeAttack(float attack) {
  envelope_.SetAttack(attack);
}

void BasicSamplerVoice::SetEnvelopeDecay(float decay) {
  envelope_.SetDecay(decay);
}

void BasicSamplerVoice::SetEnvelopeSustain(float sustain) {
  envelope_.SetSustain(sustain);
}

void BasicSamplerVoice::SetEnvelopeRelease(float release) {
  envelope_.SetRelease(release);
}

void BasicSamplerVoice::SetGain(float gain) { gain_ = gain; }

void BasicSamplerVoice::SetSamplePlayerData(const std::vector<float>& data,
                                            int frequency) {
  sample_player_.SetData(data.data(), frequency, static_cast<int>(data.size()));
}

void BasicSamplerVoice::SetSamplePlayerLoop(bool loop) {
  sample_player_.SetLoop(loop);
}

void BasicSamplerVoice::SetSamplePlayerSpeed(float speed) {
  sample_player_.SetSpeed(speed);
}

}  // namespace examples
}  // namespace barelyapi
