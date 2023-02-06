#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/enveloped_voice.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

/// Percussion control.
enum class PercussionControl : int {
  /// Pad envelope release.
  kRelease = 0,
};

/// Simple percussion instrument.
class PercussionInstrument : public GenericInstrument {
 public:
  /// Constructs new `PercussionInstrument`.
  explicit PercussionInstrument(Integer frame_rate) noexcept;

  /// Implements `GenericInstrument`.
  void Process(Real* output_samples, Integer channel_count,
               Integer frame_count) noexcept override;
  void SetControl(Integer index, Real value,
                  Real slope_per_frame) noexcept override;
  void SetData(const void* data, Integer size) noexcept override;
  void SetNoteControl(Real /*pitch*/, Integer /*index*/, Real /*value*/,
                      Real /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(Real pitch) noexcept override;
  void SetNoteOn(Real pitch) noexcept override;

  /// Returns instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

 private:
  static constexpr Integer kPadCount = 4;
  struct Pad {
    explicit Pad(Integer frame_rate) noexcept : voice(frame_rate) {}

    Real pitch = 0.0;
    EnvelopedVoice<SamplePlayer> voice;
  };
  std::array<Pad, kPadCount> pads_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
