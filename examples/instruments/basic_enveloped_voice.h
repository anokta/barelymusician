#ifndef EXAMPLES_INSTRUMENTS_BASIC_ENVEOPED_VOICE_H_
#define EXAMPLES_INSTRUMENTS_BASIC_ENVEOPED_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/instrument/voice.h"

namespace barelyapi {
namespace examples {

template <class GeneratorType>
class BasicEnvelopedVoice : public Voice {
 public:
  // Constructs new |BasicEnvelopedVoice| with the given |sample_interval|.
  //
  // @param sample_interval Sampling interval in seconds.
  explicit BasicEnvelopedVoice(float sample_interval);

  // Implements |Voice|.
  bool IsActive() const override;
  float Next() override;
  void Reset() override;
  void Start() override;
  void Stop() override;

  // Inline getter/setter functions.
  const Envelope& envelope() const { return envelope_; }
  Envelope& envelope() { return envelope_; }

  const GeneratorType& generator() const { return generator_; }
  GeneratorType& generator() { return generator_; }

  const float gain() const { return gain_; }
  void set_gain(float gain) { gain_ = gain; }

 private:
  // Voice envelope.
  Envelope envelope_;

  // Voice unit generator.
  GeneratorType generator_;

  // Voice gain.
  float gain_;
};

template <class GeneratorType>
BasicEnvelopedVoice<GeneratorType>::BasicEnvelopedVoice(float sample_interval)
    : envelope_(sample_interval), generator_(sample_interval) {}

template <class GeneratorType>
float BasicEnvelopedVoice<GeneratorType>::Next() {
  return gain_ * envelope_.Next() * generator_.Next();
}

template <class GeneratorType>
void BasicEnvelopedVoice<GeneratorType>::Reset() {
  envelope_.Reset();
  generator_.Reset();
  gain_ = 0.0f;
}

template <class GeneratorType>
bool BasicEnvelopedVoice<GeneratorType>::IsActive() const {
  return envelope_.IsActive();
}

template <class GeneratorType>
void BasicEnvelopedVoice<GeneratorType>::Start() {
  generator_.Reset();
  envelope_.Start();
}

template <class GeneratorType>
void BasicEnvelopedVoice<GeneratorType>::Stop() {
  envelope_.Stop();
}

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_ENVEOPED_VOICE_H_
