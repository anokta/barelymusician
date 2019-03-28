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

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) override;
  void NoteOff(float index) override;
  void SetFloatParam(ParamId id, float value) override;

 private:
  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;

  ModulationMatrix<float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
