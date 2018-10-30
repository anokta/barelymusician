#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_VOICE_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_VOICE_H_

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instrument/voice.h"

namespace barelyapi {
namespace examples {

class BasicSynthVoice : public Voice {
 public:
  // Constructs new |BasicSynthVoice| with the given |sample_interval|.
  //
  // @param sample_interval Sampling interval in seconds.
  BasicSynthVoice(float sample_interval);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Implements |Voice|.
  bool IsActive() const override;
  void Start() override;
  void Stop() override;

  // Sets envelope attack.
  //
  // @param attack Envelope attack.
  void SetEnvelopeAttack(float attack);

  // Sets envelope decay.
  //
  // @param decay Envelope decay.
  void SetEnvelopeDecay(float decay);

  // Sets envelope sustain.
  //
  // @param sustain Envelope sustain.
  void SetEnvelopeSustain(float sustain);

  // Sets envelope release.
  //
  // @param release Envelope release.
  void SetEnvelopeRelease(float release);

  // Sets gain.
  //
  // @param gain Linear gain.
  void SetGain(float gain);

  // Sets oscillator frequency.
  //
  // @param frequency Oscillator frequency.
  void SetOscillatorFrequency(float frequency);

  // Sets oscillator type.
  //
  // @param type Oscillator type.
  void SetOscillatorType(OscillatorType type);

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
