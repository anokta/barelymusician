#ifndef BARELYMUSICIAN_PRESETS_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_PRESETS_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/presets/instruments/enveloped_voice.h"
#include "barelymusician/presets/instruments/generic_instrument.h"

namespace barelyapi {

/// Simple polyphonic synth instrument.
class SynthInstrument : public GenericInstrument {
 public:
  /// Constructs new `SynthInstrument`.
  explicit SynthInstrument(int sample_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(void* /*data*/) noexcept override {}
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;
  void SetParameter(int index, double value, double slope) noexcept override;

  /// Returns instrument definition.
  static barely::InstrumentDefinition GetDefinition() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_INSTRUMENTS_SYNTH_INSTRUMENT_H_
