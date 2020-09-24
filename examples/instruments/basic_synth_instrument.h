#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/modulation_matrix.h"
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
};

class BasicSynthInstrument : public Instrument {
 public:
  BasicSynthInstrument(int sample_rate, int num_voices);

  // Implements |Instrument|.
  void Control(int id, float value) override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

 private:
  using BasicSynthVoice = BasicEnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;

  ModulationMatrix<float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
