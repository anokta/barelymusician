#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_

#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/polyphonic_voice.h"
#include "barelymusician/dsp/sample_player.h"

namespace barely {

/// Class that wraps the audio processing of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs a new `InstrumentProcessor`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit InstrumentProcessor(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count, int output_frame_count) noexcept;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int id, double value) noexcept;
  void SetData(const void* data, int size) noexcept;
  void SetNoteControl(double /*pitch*/, int /*id*/, double /*value*/) noexcept {}
  void SetNoteOff(double pitch) noexcept;
  void SetNoteOn(double pitch, double intensity) noexcept;

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
  double pitch_shift_ = 0.0;
  bool sampler_loop_ = false;
  GainProcessor gain_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
