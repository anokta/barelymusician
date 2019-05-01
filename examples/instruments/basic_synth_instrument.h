#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/instrument/modulation_matrix.h"
#include "barelymusician/instrument/polyphonic_voice.h"
#include "instruments/basic_synth_voice.h"

namespace barelyapi {
namespace examples {

enum BasicSynthInstrumentFloatParam {
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

  // Sets the value of a float parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Float parameter value.
  void SetFloatParam(int id, float value);

 protected:
  // Implements |Instrument|.
  void Clear() override;
  float Next(int channel) override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;

 private:
  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;

  ModulationMatrix<float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
