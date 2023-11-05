#ifndef BARELYMUSICIAN_DSP_ENVELOPED_VOICE_H_
#define BARELYMUSICIAN_DSP_ENVELOPED_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/voice.h"

namespace barely {

/// Simple enveloped voice template.
template <class GeneratorType>
class EnvelopedVoice : public Voice {
 public:
  /// Constructs new `EnvelopedVoice` with the given `frame_rate`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit EnvelopedVoice(int frame_rate) noexcept;

  /// Implements `Voice`.
  [[nodiscard]] bool IsActive() const noexcept override;
  double Next(int channel) noexcept override;
  void Start() noexcept override;
  void Stop() noexcept override;

  /// Inline getter/setter functions.
  [[nodiscard]] const Envelope& envelope() const noexcept { return envelope_; }
  [[nodiscard]] Envelope& envelope() noexcept { return envelope_; }

  [[nodiscard]] const GeneratorType& generator() const noexcept { return generator_; }
  [[nodiscard]] GeneratorType& generator() noexcept { return generator_; }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

 private:
  // Voice envelope.
  Envelope envelope_;

  // Voice unit generator.
  GeneratorType generator_;

  // Voice gain.
  double gain_ = 0.0;

  // Last output.
  double output_ = 0.0;
};

template <class GeneratorType>
EnvelopedVoice<GeneratorType>::EnvelopedVoice(int frame_rate) noexcept
    : envelope_(frame_rate), generator_(frame_rate) {}

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

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ENVELOPED_VOICE_H_
