#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

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
  static constexpr int kNumPads = 4;
  struct Pad {
    explicit Pad(int frame_rate) noexcept : voice(frame_rate) {}

    double pitch = 0.0;
    EnvelopedVoice<SamplePlayer> voice;
  };
  std::array<Pad, kNumPads> pads_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
