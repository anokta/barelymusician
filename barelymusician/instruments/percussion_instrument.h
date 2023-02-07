#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Percussion control.
enum class PercussionControl : int {
  /// Gain.
  kGain = 0,
  /// Pad envelope release.
  kRelease = 1,
};

/// Simple percussion instrument.
class PercussionInstrument : public GenericInstrument {
 public:
  /// Constructs new `PercussionInstrument`.
  explicit PercussionInstrument(int frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(double* output_samples, int channel_count,
               int frame_count) noexcept override;
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept override;
  void SetData(const void* data, int size) noexcept override;
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  static constexpr int kPadCount = 4;
  struct Pad {
    explicit Pad(int frame_rate) noexcept : voice(frame_rate) {}

    double pitch = 0.0;
    EnvelopedVoice<SamplePlayer> voice;
  };
  std::array<Pad, kPadCount> pads_;
  GainProcessor gain_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
