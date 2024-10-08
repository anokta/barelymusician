#ifndef BARELYMUSICIAN_INSTRUMENTS_ULTIMATE_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_ULTIMATE_INSTRUMENT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Returns the ultimate instrument definition.
///
/// @return Instrument definition.
BARELY_EXPORT BarelyInstrumentDefinition BarelyUltimateInstrument_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Ultimate instrument to conquer 'em all!
class UltimateInstrument : public CustomInstrument {
 public:
  /// Control enum.
  enum class Control : int32_t {
    /// Gain.
    kGain = 0,
    /// Number of voices.
    kVoiceCount,
    /// Oscillator on.
    // TODO(#139): This could be replaced by a mix value between the oscillator and sample playback.
    kOscillatorOn,
    /// Oscillator type.
    kOscillatorType,
    /// Sample player loop.
    // TODO(#139): This could be replaced by `SamplePlaybackMode` with sustained and looped modes.
    kSamplePlayerLoop,
    /// Envelope attack.
    kAttack,
    /// Envelope decay.
    kDecay,
    /// Envelope sustain.
    kSustain,
    /// Envelope release.
    kRelease,
    /// Number of controls.
    kCount,
  };

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 protected:
  /// Constructs a new `UltimateInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit UltimateInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int id, double value) noexcept final;
  void SetData(const void* data, int size) noexcept final;
  void SetNoteControl(double /*pitch*/, int /*id*/, double /*value*/) noexcept final {}
  void SetNoteOff(double pitch) noexcept final;
  void SetNoteOn(double pitch, double intensity) noexcept final;

 private:
  // TODO(#139): These should share the same voice type.
  using OscillatorVoice = EnvelopedVoice<Oscillator>;
  struct Sampler {
    using Voice = EnvelopedVoice<SamplePlayer>;
    explicit Sampler(int frame_rate) noexcept;
    double pitch = 0.0;
    PolyphonicVoice<Voice> voice;
  };
  int frame_rate_ = 0;
  bool oscillator_on_ = false;
  PolyphonicVoice<OscillatorVoice> oscillator_voice_;
  std::vector<Sampler> samplers_;
  int voice_count_ = 0;
  double attack_ = 0.0;
  double decay_ = 0.0;
  double sustain_ = 1.0;
  double release_ = 0.0;
  bool sampler_loop_ = false;
  GainProcessor gain_processor_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_ULTIMATE_INSTRUMENT_H_
