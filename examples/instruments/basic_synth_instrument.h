#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/base/frame.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/modulation_matrix.h"
#include "barelymusician/instrument/polyphonic_voice.h"
#include "instruments/basic_synth_voice.h"

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
  BasicSynthInstrument(float sample_interval, int num_voices);

  // Implements |Instrument|.
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(Frame* output) override;
  void Reset() override;

  // Sets the value of a float parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Float parameter value.
  void SetFloatParam(int id, float value);

 private:
  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;

  ModulationMatrix<float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
