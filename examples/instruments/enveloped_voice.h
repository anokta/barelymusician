#ifndef EXAMPLES_INSTRUMENTS_ENVELOPED_VOICE_H_
#define EXAMPLES_INSTRUMENTS_ENVELOPED_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/voice.h"

namespace barely::examples {

/// Simple enveloped voice template.
template <class GeneratorType>
class EnvelopedVoice : public Voice {
 public:
  /// Constructs new |EnvelopedVoice| with the given |sample_rate|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit EnvelopedVoice(int sample_rate);

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
EnvelopedVoice<GeneratorType>::EnvelopedVoice(int sample_rate)
    : envelope_(sample_rate),
      generator_(sample_rate),
      gain_(0.0f),
      output_(0.0f) {}

template <class GeneratorType>
float EnvelopedVoice<GeneratorType>::Next(int channel) {
  if (channel == 0) {
    output_ = gain_ * envelope_.Next() * generator_.Next();
  }
  return output_;
}

template <class GeneratorType>
bool EnvelopedVoice<GeneratorType>::IsActive() const {
  return envelope_.IsActive();
}

template <class GeneratorType>
void EnvelopedVoice<GeneratorType>::Start() {
  generator_.Reset();
  envelope_.Start();
}

template <class GeneratorType>
void EnvelopedVoice<GeneratorType>::Stop() {
  envelope_.Stop();
}

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_ENVELOPED_VOICE_H_
