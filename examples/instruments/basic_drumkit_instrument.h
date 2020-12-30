#ifndef EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_

#include <unordered_map>

#include "barelymusician/dsp/sample_player.h"
#include "examples/instruments/basic_enveloped_voice.h"
#include "examples/instruments/instrument.h"
#include "examples/util/wav_file.h"

namespace barelyapi {
namespace examples {

class BasicDrumkitInstrument : public Instrument {
 public:
  BasicDrumkitInstrument(int sample_rate);

  // Implements |Instrument|.
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void SetParam(int, float) override {}

  void Add(float note_index, const WavFile& wav_file);

 private:
  using BasicDrumkitVoice = BasicEnvelopedVoice<SamplePlayer>;

  const int sample_rate_;

  float gain_;

  std::unordered_map<float, BasicDrumkitVoice> voices_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_
