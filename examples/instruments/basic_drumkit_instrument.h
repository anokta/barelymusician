#ifndef EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_

#include <unordered_map>

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/modulation_matrix.h"
#include "instruments/basic_sampler_voice.h"
#include "util/wav_file.h"

namespace barelyapi {
namespace examples {

class BasicDrumkitInstrument : public Instrument {
 public:
  BasicDrumkitInstrument(float sample_interval);

  // Implements |Instrument|.
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void Reset() override;

  void Add(float note_index, const WavFile& wav_file);

 private:
  const float sample_interval_;

  float gain_;

  std::unordered_map<float, BasicSamplerVoice> voices_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_DRUMKIT_INSTRUMENT_H_
