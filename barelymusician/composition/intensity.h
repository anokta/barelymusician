#ifndef BARELYMUSICIAN_COMPOSITION_INTENSITY_H_
#define BARELYMUSICIAN_COMPOSITION_INTENSITY_H_

#include "barelymusician/barelymusician.h"

namespace barely {

// Maximum midi velocity.
inline constexpr int kMaxMidiVelocity = 127;

/// Returns the correspnding note intensity for a given midi note velocity.
///
/// @param midi Midi note velocity.
/// @return Note intensity.
Rational IntensityFromMidi(int midi) noexcept;

/// Returns the correspnding midi note velocity for a given note intensity.
///
/// @param intensity Note intensity.
/// @return Midi note velocity.
int MidiFromIntensity(Rational intensity) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_INTENSITY_H_
