#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Sampler parameter.
enum class SamplerParameter : int {
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
  explicit SamplerInstrument(int sample_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output_samples, int channel_count,
               int frame_count) noexcept override;
  void SetData(const void* data, int size) noexcept override;
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetParameter(int index, double value, double slope) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using SamplerVoice = EnvelopedVoice<SamplePlayer>;
  PolyphonicVoice<SamplerVoice> voice_;
  double root_pitch_ = 0.0;
  int sample_rate_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
