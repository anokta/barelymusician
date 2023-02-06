#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Sampler control.
enum class SamplerControl : Integer {
  /// Root pitch.
  kRootPitch = 0,
  /// Sample player loop.
  kLoop = 1,
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

/// Simple polyphonic sampler instrument.
class SamplerInstrument : public GenericInstrument {
 public:
  /// Constructs new `SamplerInstrument`.
  explicit SamplerInstrument(Integer frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(Real* output_samples, Integer channel_count,
               Integer frame_count) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(Integer index, Real value,
                  Real slope_per_frame) noexcept override;
  void SetData(const void* data, Integer size) noexcept override;
  void SetNoteControl(Real /*pitch*/, Integer /*index*/, Real /*value*/,
                      Real /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(Real pitch) noexcept override;
  void SetNoteOn(Real pitch) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SamplerVoice = EnvelopedVoice<SamplePlayer>;
  PolyphonicVoice<SamplerVoice> voice_;
  Real root_pitch_ = 0.0;
  Integer frame_rate_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
