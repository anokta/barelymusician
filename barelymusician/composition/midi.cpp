#include "barelymusician/composition/midi.h"

#include <cmath>

#include "barelymusician/composition/pitch.h"

namespace barely {

double FrequencyFromMidiNumber(int midi_number) noexcept {
  return kFrequencyA4 *
         std::pow(2.0, static_cast<double>(midi_number - kMidiNumberA4) / kSemitoneCount);
}

double IntensityFromMidiVelocity(int midi_velocity) noexcept {
  return static_cast<double>(midi_velocity) / kMaxMidiVelocity;
}

int MidiVelocityFromIntensity(double intensity) noexcept {
  return static_cast<int>(std::round(intensity * kMaxMidiVelocity));
}

}  // namespace barely
