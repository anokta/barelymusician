#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

#include <vector>

#include "barelymusician/common/custom_macros.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Default pad release in seconds.
inline constexpr double kDefaultPadRelease = 0.1;

/// Percussion instrument definition.
#define BARELY_PERCUSSION_INSTRUMENT_CONTROLS(PercussionInstrumentControl, X) \
  /* Gain. */                                                                 \
  X(PercussionInstrumentControl, Gain, 1.0, 0.0, 1.0)                         \
  /* Pad envelope release. */                                                 \
  X(PercussionInstrumentControl, Release, kDefaultPadRelease, 0.0, 60.0)
BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(
    PercussionInstrument, BARELY_PERCUSSION_INSTRUMENT_CONTROLS)

/// Simple percussion instrument.
class PercussionInstrument : public CustomInstrument {
 public:
  /// Constructs new `PercussionInstrument`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit PercussionInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetData(const void* data, int size) noexcept final;
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept final {}
  void SetNoteOff(double pitch) noexcept final;
  void SetNoteOn(double pitch, double intensity) noexcept final;

  struct Pad {
    explicit Pad(int frame_rate) noexcept : voice(frame_rate) {}

    double pitch = 0.0;
    EnvelopedVoice<SamplePlayer> voice;
  };
  int frame_rate_;
  std::vector<Pad> pads_;
  GainProcessor gain_processor_;
  double release_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
