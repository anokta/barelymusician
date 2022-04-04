#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

struct PercussionPad {
  std::vector<double> data;
  int frequency;
};
using PercussionPadMap = std::unordered_map<double, PercussionPad>;

/// Simple percussion instrument.
class PercussionInstrument : public GenericInstrument {
 public:
  /// Constructs new `PercussionInstrument`.
  explicit PercussionInstrument(int frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output, int num_channels,
               int num_frames) noexcept override;
  void SetData(const void* data, int size) noexcept override;
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;
  void SetParameter(int index, double value, double slope) noexcept override;

  /// Returns instrument definition.
  static Instrument::Definition GetDefinition() noexcept;

 private:
  int frame_rate_;
  const PercussionPadMap* pads_;
  std::unordered_map<double, EnvelopedVoice<SamplePlayer>> voices_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
