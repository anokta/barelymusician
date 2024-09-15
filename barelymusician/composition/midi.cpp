#include "barelymusician/composition/midi.h"

#include <cmath>

#include "barelymusician/composition/scale.h"

namespace barely {

namespace {

// Midi note number for A4 (middle A).
inline constexpr int kMidiNumberA4 = 69;

// Maximum midi velocity.
inline constexpr int kMaxMidiVelocity = 127;

}  // namespace

double FrequencyFromMidiNumber(int midi_number) noexcept {
  static const auto scale_definition =
      GetScaleDefinition(ScaleType::kChromatic, GetNoteFromPitch(PitchClass::kA));
  return GetNoteFromScale(scale_definition, midi_number - kMidiNumberA4);
}

double IntensityFromMidiVelocity(int midi_velocity) noexcept {
  return static_cast<double>(midi_velocity) / kMaxMidiVelocity;
}

int MidiVelocityFromIntensity(double intensity) noexcept {
  return static_cast<int>(std::round(intensity * kMaxMidiVelocity));
}

}  // namespace barely
