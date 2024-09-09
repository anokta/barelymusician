#include "barelymusician/composition/midi.h"

#include <cmath>

#include "barelymusician/composition/scale.h"

namespace barely {

double FrequencyFromMidiNumber(int midi_number) noexcept {
  static const Scale scale = CreateScale(ScaleType::kChromatic, GetNote(PitchClass::kA));
  return scale.GetNote(midi_number - kMidiNumberA4);
}

double IntensityFromMidiVelocity(int midi_velocity) noexcept {
  return static_cast<double>(midi_velocity) / kMaxMidiVelocity;
}

int MidiVelocityFromIntensity(double intensity) noexcept {
  return static_cast<int>(std::round(intensity * kMaxMidiVelocity));
}

}  // namespace barely
