#ifndef BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_

#include "barelymusician/common/custom_macros.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

// Maximum number of voices allowed to be set.
inline constexpr int kMaxSamplerVoiceCount = 64;

/// Sampler instrument definition.
#define BARELY_SAMPLER_INSTRUMENT_CONTROLS(SamplerInstrumentControl, X) \
  /* Gain. */                                                           \
  X(SamplerInstrumentControl, Gain, 1.0, 0.0, 1.0)                      \
  /* Root pitch. */                                                     \
  X(SamplerInstrumentControl, RootPitch, 0.0)                           \
  /* Sample player loop. */                                             \
  X(SamplerInstrumentControl, Loop, false)                              \
  /* Attack. */                                                         \
  X(SamplerInstrumentControl, Attack, 0.05, 0.0, 60.0)                  \
  /* Decay. */                                                          \
  X(SamplerInstrumentControl, Decay, 0.0, 0.0, 60.0)                    \
  /* Sustain. */                                                        \
  X(SamplerInstrumentControl, Sustain, 1.0, 0.0, 1.0)                   \
  /* Release. */                                                        \
  X(SamplerInstrumentControl, Release, 0.25, 0.0, 60.0)                 \
  /* Number of voices. */                                               \
  X(SamplerInstrumentControl, VoiceCount, 8, 1, kMaxSamplerVoiceCount)
BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(SamplerInstrument,
                                             BARELY_SAMPLER_INSTRUMENT_CONTROLS)

/// Simple polyphonic sampler instrument.
class SamplerInstrument : public CustomInstrument {
 public:
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

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SAMPLER_INSTRUMENT_H_
