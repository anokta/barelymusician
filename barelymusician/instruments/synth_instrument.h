#ifndef BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Synth control.
enum class SynthControl : int {
  /// Gain.
  kGain = 0,
  /// Oscillator type.
  kOscillatorType = 1,
  /// Envelope attack.
  kAttack = 2,
  /// Envelope decay.
  kDecay = 3,
  /// Envelope sustain.
  kSustain = 4,
  /// Envelope release.
  kRelease = 5,
  /// Number of voices
  kVoiceCount = 6,
};

/// Simple polyphonic synth instrument.
class SynthInstrument : public GenericInstrument {
 public:
  /// Constructs new `SynthInstrument`.
  explicit SynthInstrument(int frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output_samples, int channel_count,
               int frame_count) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept override;
  void SetData(const void* /*data*/, int /*size*/) noexcept override {}
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
  GainProcessor gain_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
