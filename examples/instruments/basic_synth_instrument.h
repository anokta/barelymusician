#ifndef EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_

#include <memory>
#include <utility>
#include <vector>

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/engine/instrument_definition.h"
#include "examples/instruments/basic_enveloped_voice.h"
#include "examples/instruments/instrument.h"

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
  // Constructs new |BasicSynthInstrument|.
  explicit BasicSynthInstrument(int sample_rate);

  // Implements |Instrument|.
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void SetCustomData(void*) override {}
  void SetParam(int id, float value) override;

  // Returns instrument definition.
  static InstrumentDefinition GetDefinition(int sample_rate);
  static std::vector<InstrumentParamDefinition> GetDefaultParams();

 private:
  using BasicSynthVoice = BasicEnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<BasicSynthVoice> voice_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SYNTH_INSTRUMENT_H_
