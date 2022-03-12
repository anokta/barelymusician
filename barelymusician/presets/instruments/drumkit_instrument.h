#ifndef BARELYMUSICIAN_PRESETS_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
#define BARELYMUSICIAN_PRESETS_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/api/instrument.h"
#include "barelymusician/presets/dsp/sample_player.h"
#include "barelymusician/presets/instruments/enveloped_voice.h"
#include "barelymusician/presets/instruments/generic_instrument.h"

namespace barelyapi {

enum DrumkitInstrumentParameter {
  kPadRelease = 0,
};

struct DrumkitPad {
  explicit DrumkitPad(std::vector<double> data, int frequency, int frame_rate)
      : data(std::move(data)), voice(frame_rate) {
    voice.generator().SetData(this->data.data(), frequency,
                              static_cast<int>(this->data.size()));
  }
  std::vector<double> data;
  EnvelopedVoice<barelyapi::SamplePlayer> voice;
};
using DrumkitPadMap = std::unordered_map<double, DrumkitPad>;

/// Simple drumkit instrument.
class DrumkitInstrument : public GenericInstrument {
 public:
  explicit DrumkitInstrument(int /*sample_rate*/) noexcept {}

  /// Implements `GenericInstrument`.
  void Process(double* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(void* data) noexcept override;
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;
  void SetParameter(int index, double value, double slope) noexcept override;

  /// Returns instrument definition.
  static barely::InstrumentDefinition GetDefinition() noexcept;

 private:
  DrumkitPadMap pads_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_INSTRUMENTS_DRUMKIT_INSTRUMENT_H_
