#ifndef EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_

#include <any>
#include <unordered_map>

#include "barelymusician/dsp/sample_player.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

/// Simple drumkit instrument.
class DrumkitInstrument : public GenericInstrument {
 public:
  DrumkitInstrument(int sample_rate) noexcept;

  /// Implements |GenericInstrument|.
  void NoteOff(float pitch) noexcept override;
  void NoteOn(float pitch, float intensity) noexcept override;
  void Process(float* output, int num_channels,
               int num_frames) noexcept override;
  void SetCustomData(std::any data) noexcept override;
  void SetParam(int, float) noexcept override {}

  static InstrumentDefinition GetDefinition() noexcept;

 private:
  using DrumkitVoice = EnvelopedVoice<SamplePlayer>;

  void Add(float pitch, const WavFile& wav_file) noexcept;

  const int sample_rate_;

  float gain_;

  std::unordered_map<float, DrumkitVoice> voices_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
