#ifndef BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Simple polyphonic synth instrument.
class SynthInstrument : public CustomInstrument {
 public:
  /// Control enum.
  enum class Control : int32_t {
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
    /// Number of voices.
    kVoiceCount = 6,
    /// Number of controls.
    kCount,
  };

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 protected:
  /// Constructs a new `SynthInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit SynthInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int id, double value) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}
  void SetNoteControl(double /*pitch*/, int /*id*/, double /*value*/) noexcept final {}
  void SetNoteOff(double pitch) noexcept final;
  void SetNoteOn(double pitch, double intensity) noexcept final;

 private:
  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
  GainProcessor gain_processor_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
