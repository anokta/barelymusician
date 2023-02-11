#ifndef BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Percussion control enum alias.
typedef int32_t BarelyPercussionControl;

/// Percussion control enum values.
enum BarelyPercussionControl_Values {
  /// Gain.
  BarelyPercussionControl_kGain = 0,
  /// Pad envelope release.
  BarelyPercussionControl_kRelease = 1,
};

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
#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instruments/custom_instrument.h"

namespace barely {

/// Percussion control enum.
enum class PercussionControl : BarelyPercussionControl {
  /// Gain.
  kGain = BarelyPercussionControl_kGain,
  /// Pad envelope release.
  kRelease = BarelyPercussionControl_kRelease,
};

/// Simple percussion instrument.
class PercussionInstrument : public CustomInstrument {
 public:
  /// Constructs new `PercussionInstrument`.
  explicit PercussionInstrument(int frame_rate) noexcept;

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int channel_count,
               int frame_count) noexcept override;
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept override;
  void SetData(const void* data, int size) noexcept override;
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/,
                      double /*slope_per_frame*/) noexcept override {}
  void SetNoteOff(double pitch) noexcept override;
  void SetNoteOn(double pitch, double intensity) noexcept override;

  /// Returns the instrument definition.
  ///
  /// @return Instrument definition.
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
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_PERCUSSION_INSTRUMENT_H_
