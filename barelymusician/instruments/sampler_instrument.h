#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Sampler control.
enum class SamplerControl : int {
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
  /// Number of voices
  kVoiceCount = 7,
};

/// Simple polyphonic sampler instrument.
class SamplerInstrument : public GenericInstrument {
 public:
  /// Constructs new `SamplerInstrument`.
  explicit SamplerInstrument(int frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output_samples, int channel_count,
               int frame_count) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept override;
  void SetData(const void* data, int size) noexcept override;
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SamplerVoice = EnvelopedVoice<SamplePlayer>;
  PolyphonicVoice<SamplerVoice> voice_;
  double root_pitch_ = 0.0;
  int frame_rate_ = 0;
  GainProcessor gain_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
