#include "barelymusician/composition/intensity.h"

#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"

namespace barely {

int MidiFromIntensity(Rational intensity) noexcept {
  return static_cast<int>(static_cast<std::int64_t>(intensity * kMaxMidiVelocity));
}

Rational IntensityFromMidi(int midi) noexcept { return Rational(midi, kMaxMidiVelocity); }

}  // namespace barely
