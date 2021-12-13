#ifndef EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include <any>
#include <vector>

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

enum SynthInstrumentParam {
  kGain = 0,
  kEnvelopeAttack = 1,
  kEnvelopeDecay = 2,
  kEnvelopeSustain = 3,
  kEnvelopeRelease = 4,
  kOscillatorType = 5,
  kNumVoices = 6,
};

/// Simple polyphonic synth instrument.
class SynthInstrument : public GenericInstrument {
 public:
  /// Constructs new |SynthInstrument|.
  explicit SynthInstrument(int sample_rate) noexcept;

  /// Implements |GenericInstrument|.
  void NoteOff(float pitch) noexcept override;
  void NoteOn(float pitch, float intensity) noexcept override;
  void Process(float* output, int num_channels,
               int num_frames) noexcept override;
  void SetCustomData(std::any /*data*/) noexcept override {}
  void SetParam(int id, float value) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;
  static ParamDefinitionMap GetParamDefinitions() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<SynthVoice> voice_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
