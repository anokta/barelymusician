#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Sampler control enum alias.
typedef int32_t BarelySamplerControl;

/// Sampler control enum values.
enum BarelySamplerControl_Values {
  /// Gain.
  BarelySamplerControl_kGain = 0,
  /// Root pitch.
  BarelySamplerControl_kRootPitch = 1,
  /// Sample player loop.
  BarelySamplerControl_kLoop = 2,
  /// Envelope attack.
  BarelySamplerControl_kAttack = 3,
  /// Envelope decay.
  BarelySamplerControl_kDecay = 4,
  /// Envelope sustain.
  BarelySamplerControl_kSustain = 5,
  /// Envelope release.
  BarelySamplerControl_kRelease = 6,
  /// Number of voices
  BarelySamplerControl_kVoiceCount = 7,
};

/// Returns the sampler instrument definition.
///
/// @return Instrument definition.
BARELY_EXPORT BarelyInstrumentDefinition
BarelySamplerInstrument_GetDefinition();

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

/// Sampler control enum.
enum class SamplerControl : BarelySamplerControl {
  /// Gain.
  kGain = BarelySamplerControl_kGain,
  /// Root pitch.
  kRootPitch = BarelySamplerControl_kRootPitch,
  /// Sample player loop.
  kLoop = BarelySamplerControl_kLoop,
  /// Envelope attack.
  kAttack = BarelySamplerControl_kAttack,
  /// Envelope decay.
  kDecay = BarelySamplerControl_kDecay,
  /// Envelope sustain.
  kSustain = BarelySamplerControl_kSustain,
  /// Envelope release.
  kRelease = BarelySamplerControl_kRelease,
  /// Number of voices
  kVoiceCount = BarelySamplerControl_kVoiceCount,
};

/// Simple polyphonic sampler instrument.
class SamplerInstrument : public CustomInstrument {
 public:
  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  static InstrumentDefinition GetDefinition() noexcept;

 protected:
  /// Constructs new `SamplerInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit SamplerInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept final;
  void SetData(const void* data, int size) noexcept final;
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept final {}
  void SetNoteOff(double pitch) noexcept final;
  void SetNoteOn(double pitch, double intensity) noexcept final;

  using SamplerVoice = EnvelopedVoice<SamplePlayer>;
  PolyphonicVoice<SamplerVoice> voice_;
  double root_pitch_ = 0.0;
  GainProcessor gain_processor_;
};

#endif  // __cplusplus
}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
