#include "barelymusician/composition/pitch.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <span>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"

namespace barely {

int MidiFromPitch(Rational pitch) noexcept {
  return static_cast<int>(static_cast<int64_t>(kSemitoneCount * pitch)) + kMidiA0;
}

Rational PitchFromMidi(int midi) noexcept { return {midi - kMidiA0, kSemitoneCount}; }

Rational PitchFromScale(std::span<const Rational> scale, int index) noexcept {
  if (!scale.empty()) {
    const int scale_length = static_cast<int>(scale.size());
    const int octave_offset =
        static_cast<int>(std::floor(static_cast<float>(index) / static_cast<float>(scale_length)));
    const int scale_offset = index - octave_offset * scale_length;
    assert(scale_offset >= 0);
    return octave_offset + scale[scale_offset];
  }
  return 0;
}

}  // namespace barely
