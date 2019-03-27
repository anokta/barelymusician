#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include "barelymusician/instrument/modulation_matrix.h"
#include "barelymusician/instrument/polyphonic_voice.h"
#include "instruments/basic_synth_voice.h"

namespace barelyapi {
namespace examples {

class BasicSynthInstrument : public Instrument {
 public:
  enum class InstrumentFloatParam {
    kGain = 0,
    kEnvelopeAttack,
    kEnvelopeDecay,
    kEnvelopeSustain,
    kEnvelopeRelease,
    kOscillatorType,
  };

  // Constructs new |BasicSynthInstrument| with the given number of voices.
  //
  // @param Maximum number of simultaneous voices.
  BasicSynthInstrument(float sample_interval, int num_voices);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) override;
  void NoteOff(float index) override;
  bool SetFloatParam(ParamId id, float value) override;

 private:
  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;

  ModulationMatrix<float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
