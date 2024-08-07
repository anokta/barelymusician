#ifndef BARELYMUSICIAN_COMPOSITION_MIDI_H_
#define BARELYMUSICIAN_COMPOSITION_MIDI_H_

namespace barely {

// Midi note number for A4 (middle A).
inline constexpr int kMidiNumberA4 = 69;

// Maximum midi note number.
inline constexpr int kMaxMidiNumber = 127;

// Maximum midi velocity.
inline constexpr int kMaxMidiVelocity = 127;

/// Returns the correspnding note intensity for a given midi note velocity.
///
/// @param midi Midi note velocity.
/// @return Note intensity.
double IntensityFromMidiVelocity(int midi_velocity) noexcept;

/// Returns the correspnding midi note number for a given pitch.
///
/// @param pitch Note pitch.
/// @return Midi note number.
int MidiNumberFromPitch(double pitch) noexcept;

/// Returns the correspnding midi note velocity for a given note intensity.
///
/// @param intensity Note intensity.
/// @return Midi note velocity.
int MidiVelocityFromIntensity(double intensity) noexcept;

/// Returns the correspnding note pitch for a given midi note number.
///
/// @param midi Midi note number.
/// @return Note pitch.
double PitchFromMidiNumber(int midi_number) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_MIDI_H_
