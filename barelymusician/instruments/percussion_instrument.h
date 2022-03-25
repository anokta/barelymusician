#ifndef BARELYMUSICIAN_ENGINES_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINES_PERCUSSION_INSTRUMENT_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

struct PercussionPad {
  explicit PercussionPad(std::vector<double> data, int frequency,
                         int frame_rate)
      : data(std::move(data)), voice(frame_rate) {
    voice.generator().SetData(this->data.data(), frequency,
                              static_cast<int>(this->data.size()));
  }
  std::vector<double> data;
  EnvelopedVoice<barelyapi::SamplePlayer> voice;
};
using PercussionPadMap = std::unordered_map<double, PercussionPad>;

/// Simple percussion instrument.
class PercussionInstrument : public GenericInstrument {
 public:
  explicit PercussionInstrument(int /*sample_rate*/) noexcept {}

  /// Implements `GenericInstrument`.
  void Process(double* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(void* data) noexcept override;
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;
  void SetParameter(int index, double value, double slope) noexcept override;

  /// Returns instrument definition.
  static Instrument::Definition GetDefinition() noexcept;

 private:
  PercussionPadMap pads_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINES_PERCUSSION_INSTRUMENT_H_
