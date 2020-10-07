#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_definition.h"
#include "instruments/basic_enveloped_voice.h"
#include "voice/polyphonic_voice.h"

namespace barelyapi {
namespace examples {

enum BasicSynthInstrumentParam {
  kGain = 0,
  kEnvelopeAttack = 1,
  kEnvelopeDecay = 2,
  kEnvelopeSustain = 3,
  kEnvelopeRelease = 4,
  kOscillatorType = 5,
  kNumVoices = 6,
};

class BasicSynthInstrument : public Instrument {
 public:
  explicit BasicSynthInstrument(int sample_rate);

  // Implements |Instrument|.
  void Control(std::int64_t id, float value) override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

  static InstrumentDefinition GetDefinition(int sample_rate);

 private:
  using BasicSynthVoice = BasicEnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
