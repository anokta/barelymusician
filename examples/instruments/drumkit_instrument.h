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
  DrumkitInstrument(int sample_rate);

  /// Implements |GenericInstrument|.
  void NoteOff(float pitch) override;
  void NoteOn(float pitch, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void SetCustomData(std::any data) override;
  void SetParam(int, float) override {}

  static InstrumentDefinition GetDefinition();

 private:
  using DrumkitVoice = EnvelopedVoice<SamplePlayer>;

  void Add(float pitch, const WavFile& wav_file);

  const int sample_rate_;

  float gain_;

  std::unordered_map<float, DrumkitVoice> voices_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
