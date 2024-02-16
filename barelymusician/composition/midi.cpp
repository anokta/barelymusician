#include "barelymusician/composition/midi.h"

#include <cmath>

#include "barelymusician/composition/pitch.h"

namespace barely {

double IntensityFromMidiVelocity(int midi_velocity) noexcept {
  return static_cast<double>(midi_velocity) / kMaxMidiVelocity;
}

int MidiNumberFromPitch(double pitch) noexcept {
  return static_cast<int>(std::round(kSemitoneCount * pitch)) + kMidiNumberA4;
}

int MidiVelocityFromIntensity(double intensity) noexcept {
  return static_cast<int>(std::round(intensity * kMaxMidiVelocity));
}

double PitchFromMidiNumber(int midi_number) noexcept {
  return static_cast<double>(midi_number - kMidiNumberA4) / kSemitoneCount;
}

}  // namespace barely
