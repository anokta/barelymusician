#ifndef EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

enum SynthInstrumentParameter {
  kEnvelopeAttack = 0,
  kEnvelopeDecay = 1,
  kEnvelopeSustain = 2,
  kEnvelopeRelease = 3,
  kOscillatorType = 4,
  kNumVoices = 5,
};

/// Simple polyphonic synth instrument.
class SynthInstrument : public GenericInstrument {
 public:
  /// Constructs new `SynthInstrument`.
  explicit SynthInstrument(int sample_rate) noexcept;

  /// Implements `GenericInstrument`.
  void NoteOff(float pitch) noexcept override;
  void NoteOn(float pitch, float intensity) noexcept override;
  void Process(float* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(void* /*data*/) noexcept override {}
  void SetParameter(int index, float value) noexcept override;

  /// Returns instrument definition.
  static BarelyInstrumentDefinition GetDefinition() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<barelyapi::Oscillator>;

  barelyapi::PolyphonicVoice<SynthVoice> voice_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_SYNTH_INSTRUMENT_H_
