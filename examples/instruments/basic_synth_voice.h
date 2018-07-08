#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_VOICE_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instrument/voice.h"

namespace barelyapi {
namespace examples {

class BasicSynthVoice : public Voice {
public:
  // Float parameters of the voice.
  enum class VoiceFloatParam {
    kEnvelopeAttack = 0,
    kEnvelopeDecay,
    kEnvelopeSustain,
    kEnvelopeRelease,
    kOscillatorFrequency,
    kOscillatorType,
  };

  BasicSynthVoice(float sample_interval);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Implements |Voice|.
  bool IsActive() const override;
  void Start(float index, float intensity) override;
  void Stop() override;

  // Sets the value of the given float parameter.
  //
  // @param param Float parameter type.
  // @param value Float parameter value.
  void SetFloatParam(VoiceFloatParam type, float value);

private:
  // Voice envelope.
  Envelope envelope_;

  // Voice unit generator.
  Oscillator oscillator_;

  // Voice gain.
  float gain_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_VOICE_H_
