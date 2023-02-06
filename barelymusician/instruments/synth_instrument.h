#ifndef BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Synth control.
enum class SynthControl : int {
  /// Oscillator type.
  kOscillatorType = 0,
  /// Envelope attack.
  kAttack = 1,
  /// Envelope decay.
  kDecay = 2,
  /// Envelope sustain.
  kSustain = 3,
  /// Envelope release.
  kRelease = 4,
  /// Number of voices
  kVoiceCount = 5,
};

/// Synth note control.
enum class SynthNoteControl : Integer {
  /// Intensity.
  kIntensity = 0,
};

/// Simple polyphonic synth instrument.
class SynthInstrument : public GenericInstrument {
 public:
  /// Constructs new `SynthInstrument`.
  explicit SynthInstrument(Integer frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(Real* output_samples, Integer channel_count,
               Integer frame_count) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(Integer index, Real value,
                  Real slope_per_frame) noexcept override;
  void SetData(const void* /*data*/, Integer /*size*/) noexcept override {}
  void SetNoteControl(Real /*pitch*/, Integer /*index*/, Real /*value*/,
                      Real /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(Real pitch) noexcept override;
  void SetNoteOn(Real pitch) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
