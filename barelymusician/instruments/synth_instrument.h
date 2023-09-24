#ifndef BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_

#include "barelymusician/common/custom_macros.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

// Maximum number of voices allowed to be set.
inline constexpr int kMaxSynthVoiceCount = 64;

/// Synth instrument definition.
#define BARELY_SYNTH_INSTRUMENT_CONTROLS(SynthInstrumentControl, X) \
  /* Gain. */                                                       \
  X(SynthInstrumentControl, Gain, 1.0, 0.0, 1.0)                    \
  /* Oscillator type. */                                            \
  X(SynthInstrumentControl, OscillatorType,                         \
    static_cast<double>(OscillatorType::kSine), 0.0,                \
    static_cast<double>(OscillatorType::kNoise))                    \
  /* Attack. */                                                     \
  X(SynthInstrumentControl, Attack, 0.05, 0.0, 60.0)                \
  /* Decay. */                                                      \
  X(SynthInstrumentControl, Decay, 0.0, 0.0, 60.0)                  \
  /* Sustain. */                                                    \
  X(SynthInstrumentControl, Sustain, 1.0, 0.0, 1.0)                 \
  /* Release. */                                                    \
  X(SynthInstrumentControl, Release, 0.25, 0.0, 60.0)               \
  /* Number of voices. */                                           \
  X(SynthInstrumentControl, VoiceCount, 8, 1, kMaxSynthVoiceCount)
BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(SynthInstrument,
                                             BARELY_SYNTH_INSTRUMENT_CONTROLS)

/// Simple polyphonic synth instrument.
class SynthInstrument : public CustomInstrument {
 public:
  /// Constructs new `SynthInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit SynthInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept final {}
  void SetNoteOff(double pitch) noexcept final;
  void SetNoteOn(double pitch, double intensity) noexcept final;

  using SynthVoice = EnvelopedVoice<Oscillator>;
  PolyphonicVoice<SynthVoice> voice_;
  GainProcessor gain_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_SYNTH_INSTRUMENT_H_
