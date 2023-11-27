#include "barelymusician/composition/intensity.h"

#include <cmath>

namespace barely {

int MidiFromIntensity(double intensity) noexcept {
  return static_cast<int>(std::round(intensity * kMaxMidiVelocity));
}

double IntensityFromMidi(int midi) noexcept { return static_cast<double>(midi) / kMaxMidiVelocity; }

}  // namespace barely
