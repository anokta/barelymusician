#ifndef EXAMPLES_INSTRUMENTS_BASIC_ENVEOPED_VOICE_H_
#define EXAMPLES_INSTRUMENTS_BASIC_ENVEOPED_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/voice.h"

namespace barelyapi {
namespace examples {

template <class GeneratorType>
class BasicEnvelopedVoice : public Voice {
 public:
  /// Constructs new |BasicEnvelopedVoice| with the given |sample_rate|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit BasicEnvelopedVoice(int sample_rate);

  /// Implements |Voice|.
  bool IsActive() const override;
  float Next(int channel) override;
  void Start() override;
  void Stop() override;

  /// Inline getter/setter functions.
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

  // Last output.
  float output_;
};

template <class GeneratorType>
BasicEnvelopedVoice<GeneratorType>::BasicEnvelopedVoice(int sample_rate)
    : envelope_(sample_rate),
      generator_(sample_rate),
      gain_(0.0f),
      output_(0.0f) {}

template <class GeneratorType>
float BasicEnvelopedVoice<GeneratorType>::Next(int channel) {
  if (channel == 0) {
    output_ = gain_ * envelope_.Next() * generator_.Next();
  }
  return output_;
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
