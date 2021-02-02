#ifndef EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include <memory>
#include <utility>
#include <vector>

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/instrument.h"

namespace barelyapi::examples {

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
class SynthInstrument : public Instrument {
 public:
  /// Constructs new |SynthInstrument|.
  explicit SynthInstrument(int sample_rate);

  /// Implements |Instrument|.
  void NoteOff(float pitch) override;
  void NoteOn(float pitch, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void SetCustomData(void*) override {}
  void SetParam(int id, float value) override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition();
  static std::vector<InstrumentParamDefinition> GetDefaultParams();

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;

  float gain_;

  PolyphonicVoice<SynthVoice> voice_;
};

}  // namespace barelyapi::examples

#endif  // EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
