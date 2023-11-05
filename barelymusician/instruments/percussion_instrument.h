#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Returns the percussion instrument definition.
///
/// @return Instrument definition.
BARELY_EXPORT BarelyInstrumentDefinition
BarelyPercussionInstrument_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <vector>

#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Simple percussion instrument.
class PercussionInstrument : public CustomInstrument {
 public:
  /// Control enum.
  enum class Control : int {
    /// Gain.
    kGain = 0,
    /// Pad envelope release.
    kRelease = 1,
  };

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
  static InstrumentDefinition GetDefinition() noexcept;

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
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
