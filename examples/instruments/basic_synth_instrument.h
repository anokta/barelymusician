#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/polyphonic_voice.h"
#include "instruments/basic_enveloped_voice.h"

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
  void Control(int id, float value) override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

  static InstrumentDefinition GetDefinition();

 private:
  using BasicSynthVoice = BasicEnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
