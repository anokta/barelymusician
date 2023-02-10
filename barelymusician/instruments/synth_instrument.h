#ifndef BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Synth control enum alias.
typedef int32_t BarelySynthControl;

/// Synth control enum values.
enum BarelySynthControl_Values {
  /// Gain.
  BarelySynthControl_kGain = 0,
  /// Oscillator type.
  BarelySynthControl_kOscillatorType = 1,
  /// Envelope attack.
  BarelySynthControl_kAttack = 2,
  /// Envelope decay.
  BarelySynthControl_kDecay = 3,
  /// Envelope sustain.
  BarelySynthControl_kSustain = 4,
  /// Envelope release.
  BarelySynthControl_kRelease = 5,
  /// Number of voices.
  BarelySynthControl_kVoiceCount = 6,
};

/// Returns the synth instrument definition.
///
/// @return Instrument definition.
BARELY_EXPORT BarelyInstrumentDefinition BarelySynthInstrument_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Synth control.
enum class SynthControl : int {
  /// Gain.
  kGain = BarelySynthControl_kGain,
  /// Oscillator type.
  kOscillatorType = BarelySynthControl_kOscillatorType,
  /// Envelope attack.
  kAttack = BarelySynthControl_kAttack,
  /// Envelope decay.
  kDecay = BarelySynthControl_kDecay,
  /// Envelope sustain.
  kSustain = BarelySynthControl_kSustain,
  /// Envelope release.
  kRelease = BarelySynthControl_kRelease,
  /// Number of voices.
  kVoiceCount = BarelySynthControl_kVoiceCount,
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

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
  GainProcessor gain_processor_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
