#ifndef EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
#define EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_

#include <any>
#include <unordered_map>
#include <vector>

#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/enveloped_voice.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

enum DrumkitInstrumentParam {
  kPadRelease = 0,
};

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
  void SetParam(barelyapi::ParamId, float) noexcept override;

  /// Returns instrument definition.
  static barelyapi::InstrumentDefinition GetDefinition() noexcept;

 private:
  struct DrumkitPad {
    explicit DrumkitPad(const WavFile& wav_file, int sample_rate)
        : data(wav_file.GetData()), voice(sample_rate) {
      voice.generator().SetData(data.data(), wav_file.GetSampleRate(),
                                static_cast<int>(data.size()));
    }
    std::vector<float> data;
    EnvelopedVoice<barelyapi::SamplePlayer> voice;
  };

  int sample_rate_;

  std::unordered_map<float, DrumkitPad> pads_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
