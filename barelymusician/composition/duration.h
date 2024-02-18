#ifndef BARELYMUSICIAN_COMPOSITION_DURATION_H_
#define BARELYMUSICIAN_COMPOSITION_DURATION_H_

namespace barely {

/// Common note values in relation to quarter note beat duration.
inline constexpr double kQuarterNotesPerBeat = 1.0;
inline constexpr double kEighthNotesPerBeat = 2.0;
inline constexpr double kEighthTripletNotesPerBeat = 3.0;
inline constexpr double kSixteenthNotesPerBeat = 4.0;
inline constexpr double kSixteenthTripletNotesPerBeat = 6.0;
inline constexpr double kThirtySecondNotesPerBeat = 8.0;
inline constexpr double kThirtySecondTripletNotesPerBeat = 12.0;

/// Returns quantized position.
///
/// @param position Original position in beats.
/// @param resolution Quantization resolution.
/// @param amount Quantization amount.
/// @return Quantized position in beats.
double QuantizePosition(double position, double resolution, double amount = 1.0) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_DURATION_H_
