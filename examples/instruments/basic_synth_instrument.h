#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include <unordered_map>

#include "barelymusician/instrument/polyphonic_instrument.h"
#include "instruments/basic_synth_voice.h"

namespace barelyapi {
namespace examples {

class BasicSynthInstrument : public PolyphonicInstrument<BasicSynthVoice> {
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
  float Next() final override;

  // Implements |Instrument|.
  float GetFloatParam(int id) const override;

  void SetFloatParam(int id, float value) override;

 private:
  // TODO(#14): Create a helper class to generalize the modulation matrix
  // implementation.
  void InitializeModulationMatrix();

  // TODO(#13): TOO MUCH REDUNDANCY!! Refactor all this (combined with #14?).
  void UpdateParam(InstrumentFloatParam param, float value);

  float gain_;

  // Parameter modulation matrix.
  std::unordered_map<InstrumentFloatParam, float> modulation_matrix_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
