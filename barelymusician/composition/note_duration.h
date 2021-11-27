#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_

#include <variant>

namespace barely {

/// Common note values in relation to quarter note beat duration.
inline constexpr int kNumQuarterNotesPerBeat = 1;
inline constexpr int kNumEighthNotesPerBeat = 2;
inline constexpr int kNumEighthTripletNotesPerBeat = 3;
inline constexpr int kNumSixteenthNotesPerBeat = 4;
inline constexpr int kNumSixteenthTripletNotesPerBeat = 6;
inline constexpr int kNumThirtySecondNotesPerBeat = 8;
inline constexpr int kNumThirtySecondTripletNotesPerBeat = 12;

/// Note duration type.
using NoteDuration = std::variant<double>;

/// Returns quantized position for a given number of beat steps.
///
/// @param step Quantized step.
/// @param num_steps Number of steps per beat.
/// @return Quantized position in beats.
double GetPosition(int step, int num_steps) noexcept;

/// Returns quantized position.
///
/// @param position Original position in beats.
/// @param resolution Quantization resolution.
/// @param amount Quantization amount.
/// @return Quantized position in beats.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_
