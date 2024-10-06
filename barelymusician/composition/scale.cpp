#include "barelymusician/composition/scale.h"

#include <cassert>
#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace {

// Number of semitones in an octave.
constexpr int kSemitoneCount = 12;

// Number of pitches in a heptatonic scale.
constexpr int kHeptatonicScaleCount = 7;

/// Common musical scale ratios.
constexpr std::array<double, kSemitoneCount> kSemitones = {
    0.0 / static_cast<double>(kSemitoneCount),  1.0 / static_cast<double>(kSemitoneCount),
    2.0 / static_cast<double>(kSemitoneCount),  3.0 / static_cast<double>(kSemitoneCount),
    4.0 / static_cast<double>(kSemitoneCount),  5.0 / static_cast<double>(kSemitoneCount),
    6.0 / static_cast<double>(kSemitoneCount),  7.0 / static_cast<double>(kSemitoneCount),
    8.0 / static_cast<double>(kSemitoneCount),  9.0 / static_cast<double>(kSemitoneCount),
    10.0 / static_cast<double>(kSemitoneCount), 11.0 / static_cast<double>(kSemitoneCount),
};
constexpr std::array<double, kHeptatonicScaleCount> kDiatonicPitches = {
    kSemitones[0], kSemitones[2], kSemitones[4],  kSemitones[5],
    kSemitones[7], kSemitones[9], kSemitones[11],
};
constexpr std::array<double, kHeptatonicScaleCount> kHarmonicMinorPitches = {
    kSemitones[0], kSemitones[2], kSemitones[3],  kSemitones[5],
    kSemitones[7], kSemitones[8], kSemitones[11],
};

}  // namespace

bool BarelyScale_GetPitch(const BarelyScaleDefinition* definition, int32_t degree,
                          double* out_pitch) {
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
  *out_pitch = definition->root_pitch + static_cast<double>(octave) + definition->pitches[index] -
               definition->pitches[definition->mode];
  return true;
}

bool Barely_GetScaleDefinition(BarelyScaleType type, double root_pitch,
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
