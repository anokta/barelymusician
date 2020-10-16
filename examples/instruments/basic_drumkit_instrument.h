#ifndef EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_

#include <unordered_map>

#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/engine/instrument.h"
#include "instruments/basic_enveloped_voice.h"
#include "util/wav_file.h"

namespace barelyapi {
namespace examples {

class BasicDrumkitInstrument : public Instrument {
 public:
  BasicDrumkitInstrument(int sample_rate);

  // Implements |Instrument|.
  void Control(int, float) override {}
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void PrepareToPlay(int) override {}
  void Process(float* output, int num_channels, int num_frames) override;

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
