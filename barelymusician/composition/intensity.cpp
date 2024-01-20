#include "barelymusician/composition/intensity.h"

#include <algorithm>

namespace barely {

int MidiFromIntensity(float intensity) noexcept {
  return std::clamp(static_cast<int>(intensity * static_cast<float>(kMaxMidiVelocity)), 0, 127);
}

float IntensityFromMidi(int midi) noexcept {
  return std::clamp(static_cast<float>(midi) / static_cast<float>(kMaxMidiVelocity), 0.0f, 1.0f);
}

}  // namespace barely
