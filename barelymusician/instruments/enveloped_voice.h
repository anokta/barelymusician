#ifndef BARELYMUSICIAN_INSTRUMENTS_ENVELOPED_VOICE_H_
#define BARELYMUSICIAN_INSTRUMENTS_ENVELOPED_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/voice.h"

namespace barelyapi {

/// Simple enveloped voice template.
template <class GeneratorType>
class EnvelopedVoice : public barelyapi::Voice {
 public:
  /// Constructs new `EnvelopedVoice` with the given `sample_rate`.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit EnvelopedVoice(int sample_rate) noexcept;

  /// Implements `Voice`.
  [[nodiscard]] bool IsActive() const noexcept override;
  double Next(int channel) noexcept override;
  void Start() noexcept override;
  void Stop() noexcept override;

  /// Inline getter/setter functions.
  [[nodiscard]] const barelyapi::Envelope& envelope() const noexcept {
    return envelope_;
  }
  [[nodiscard]] barelyapi::Envelope& envelope() noexcept { return envelope_; }

  [[nodiscard]] const GeneratorType& generator() const noexcept {
    return generator_;
  }
  [[nodiscard]] GeneratorType& generator() noexcept { return generator_; }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

 private:
  // Voice envelope.
  barelyapi::Envelope envelope_;

  // Voice unit generator.
  GeneratorType generator_;

  // Voice gain.
  double gain_;

  // Last output.
  double output_;
};

template <class GeneratorType>
EnvelopedVoice<GeneratorType>::EnvelopedVoice(int sample_rate) noexcept
    : envelope_(sample_rate),
      generator_(sample_rate),
      gain_(0.0),
      output_(0.0) {}

template <class GeneratorType>
double EnvelopedVoice<GeneratorType>::Next(int channel) noexcept {
  if (channel == 0) {
    output_ = gain_ * envelope_.Next() * generator_.Next();
  }
  return output_;
}

template <class GeneratorType>
bool EnvelopedVoice<GeneratorType>::IsActive() const noexcept {
  return envelope_.IsActive();
}

template <class GeneratorType>
void EnvelopedVoice<GeneratorType>::Start() noexcept {
  generator_.Reset();
  envelope_.Start();
}

template <class GeneratorType>
void EnvelopedVoice<GeneratorType>::Stop() noexcept {
  envelope_.Stop();
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENTS_ENVELOPED_VOICE_H_
