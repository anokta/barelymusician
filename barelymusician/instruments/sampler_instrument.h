#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Returns the sampler instrument definition.
///
/// @return Instrument definition.
BARELY_EXPORT BarelyInstrumentDefinition BarelySamplerInstrument_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Simple polyphonic sampler instrument.
class SamplerInstrument : public CustomInstrument {
 public:
  /// Control enum.
  enum class Control : int {
    /// Gain.
    kGain = 0,
    /// Root pitch.
    kRootPitch = 1,
    /// Sample player loop.
    kLoop = 2,
    /// Envelope attack.
    kAttack = 3,
    /// Envelope decay.
    kDecay = 4,
    /// Envelope sustain.
    kSustain = 5,
    /// Envelope release.
    kRelease = 6,
    /// Number of voices.
    kVoiceCount = 7,
    /// Number of controls.
    kCount,
  };

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 protected:
  /// Constructs new `SamplerInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit SamplerInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int control_id, double value) noexcept final;
  void SetData(const void* data, int size) noexcept final;
  void SetNoteControl(int /*note_id*/, int /*control_id*/, double /*value*/) noexcept final {}
  void SetNoteOff(int note_id) noexcept final;
  void SetNoteOn(int note_id, double pitch, double intensity) noexcept final;

 private:
  using SamplerVoice = EnvelopedVoice<SamplePlayer>;
  PolyphonicVoice<SamplerVoice> voice_;
  double root_pitch_ = 0.0;
  GainProcessor gain_processor_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
