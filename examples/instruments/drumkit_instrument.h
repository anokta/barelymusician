#ifndef EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "examples/common/wav_file.h"
#include "examples/dsp/sample_player.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

enum DrumkitInstrumentParameter {
  kPadRelease = 0,
};

struct DrumkitPad {
  explicit DrumkitPad(const WavFile& wav_file, int sample_rate)
      : data(wav_file.GetData()), voice(sample_rate) {
    voice.generator().SetData(data.data(), wav_file.GetSampleRate(),
                              static_cast<int>(data.size()));
  }
  std::vector<float> data;
  EnvelopedVoice<barelyapi::SamplePlayer> voice;
};
using DrumkitPadMap = std::unordered_map<float, DrumkitPad>;

/// Simple drumkit instrument.
class DrumkitInstrument : public GenericInstrument {
 public:
  explicit DrumkitInstrument(int /*sample_rate*/) noexcept {}

  /// Implements `GenericInstrument`.
  void NoteOff(float pitch) noexcept override;
  void NoteOn(float pitch, float intensity) noexcept override;
  void Process(float* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(void* data) noexcept override;
  void SetParameter(int index, float value) noexcept override;

  /// Returns instrument definition.
  static BarelyInstrumentDefinition GetDefinition() noexcept;

 private:
  DrumkitPadMap pads_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
