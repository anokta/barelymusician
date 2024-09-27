#include "barelymusician/composition/scale.h"

#include <cassert>
#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace {

// Number of semitones in an octave.
inline constexpr int kSemitoneCount = 12;

// Number of pitches in a heptatonic scale.
inline constexpr int kHeptatonicScaleCount = 7;

/// Common musical scale ratios.
inline constexpr std::array<int, kSemitoneCount> kSemitones = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
};
inline constexpr std::array<int, kHeptatonicScaleCount> kDiatonicPitches = {
    2, 4, 5, 7, 9, 11, 12,
};
inline constexpr std::array<int, kHeptatonicScaleCount> kHarmonicMinorPitches = {
    2, 3, 5, 7, 8, 11, 12,
};

}  // namespace

bool BarelyScale_GetPitch(const BarelyScaleDefinition* definition, int32_t degree,
                          int32_t* out_pitch) {
  if (definition == nullptr) return false;
  if (definition->pitches == nullptr || definition->pitch_count == 0) return false;
  if (definition->mode < 0 || definition->mode >= definition->pitch_count) return false;
  if (out_pitch == nullptr) return false;

  const int scale_degree = degree + definition->mode;
  const int pitch_count = static_cast<int>(definition->pitch_count);
  const int octave = static_cast<int>(
      std::floor(static_cast<double>(scale_degree) / static_cast<double>(pitch_count)));
  const int index = scale_degree - octave * pitch_count;
  assert(index >= 0 && index < pitch_count);
  *out_pitch = definition->root_pitch + octave * definition->pitches[pitch_count - 1];
  if (index > 0) {
    *out_pitch += definition->pitches[index - 1];
  }
  if (definition->mode > 0) {
    *out_pitch += definition->pitches[definition->mode - 1];
  }
  return true;
}

bool Barely_GetScaleDefinition(BarelyScaleType type, int32_t root_pitch,
                               BarelyScaleDefinition* out_definition) {
  if (type < 0 || type >= static_cast<int>(barely::ScaleType::kCount)) return false;
  if (out_definition == nullptr) return false;

  switch (static_cast<barely::ScaleType>(type)) {
    case barely::ScaleType::kChromatic:
      *out_definition = barely::ScaleDefinition{kSemitones, root_pitch};
      return true;
    case barely::ScaleType::kDiatonic:
      *out_definition = barely::ScaleDefinition{kDiatonicPitches, root_pitch};
      return true;
    case barely::ScaleType::kHarmonicMinor:
      *out_definition = barely::ScaleDefinition{kHarmonicMinorPitches, root_pitch};
      return true;
    default:
      return false;
  }
}
