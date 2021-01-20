#ifndef EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_

#include <memory>
#include <unordered_map>

#include "barelymusician/dsp/sample_player.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/instrument.h"
#include "examples/util/wav_file.h"

namespace barelyapi {
namespace examples {

/// Simple drumkit instrument.
class DrumkitInstrument : public Instrument {
 public:
  DrumkitInstrument(int sample_rate);

  /// Implements |Instrument|.
  void NoteOff(float pitch) override;
  void NoteOn(float pitch, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void SetCustomData(void* data) override;
  void SetParam(int, float) override {}

  static InstrumentDefinition GetDefinition(int sample_rate);

 private:
  using DrumkitVoice = EnvelopedVoice<SamplePlayer>;

  void Add(float pitch, const WavFile& wav_file);

  const int sample_rate_;

  float gain_;

  std::unordered_map<float, DrumkitVoice> voices_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
