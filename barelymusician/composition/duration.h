#ifndef BARELYMUSICIAN_COMPOSITION_DURATION_H_
#define BARELYMUSICIAN_COMPOSITION_DURATION_H_

namespace barely {

/// Common note values in relation to quarter note beat duration.
inline constexpr int kQuarterNotesPerBeat = 1;
inline constexpr int kEighthNotesPerBeat = 2;
inline constexpr int kEighthTripletNotesPerBeat = 3;
inline constexpr int kSixteenthNotesPerBeat = 4;
inline constexpr int kSixteenthTripletNotesPerBeat = 6;
inline constexpr int kThirtySecondNotesPerBeat = 8;
inline constexpr int kThirtySecondTripletNotesPerBeat = 12;

/// Returns quantized position for a given number of beat steps.
///
/// @param step Quantized step.
/// @param step_count Number of steps per beat.
/// @return Quantized position in beats.
double GetPosition(int step, int step_count) noexcept;

/// Returns quantized position.
///
/// @param position Original position in beats.
/// @param resolution Quantization resolution.
/// @param amount Quantization amount.
/// @return Quantized position in beats.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_DURATION_H_
