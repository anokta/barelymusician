#include "barelymusician/composition/scale.h"

#include <cassert>
#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace {

// A0 reference frequency.
inline constexpr double kFrequencyA0 = 27.5;

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr int kSemitoneCount = static_cast<int>(barely::PitchClass::kCount);

/// Semitone ratios of an octave.
inline constexpr std::array<double, kSemitoneCount> kSemitoneRatios = {
    1.0594630943592953,  // std::pow(2.0, 1.0 / 12.0)
    1.122462048309373,   // std::pow(2.0, 2.0 / 12.0)
    1.189207115002721,   // std::pow(2.0, 3.0 / 12.0)
    1.2599210498948732,  // std::pow(2.0, 4.0 / 12.0)
    1.3348398541700344,  // std::pow(2.0, 5.0 / 12.0)
    1.4142135623730951,  // std::pow(2.0, 6.0 / 12.0)
    1.4983070768766815,  // std::pow(2.0, 7.0 / 12.0)
    1.5874010519681994,  // std::pow(2.0, 8.0 / 12.0)
    1.681792830507429,   // std::pow(2.0, 9.0 / 12.0)
    1.7817974362806785,  // std::pow(2.0, 10.0 / 12.0)
    1.8877486253633868,  // std::pow(2.0, 11.0 / 12.0)
    2.0,                 // std::pow(2.0, 12.0 / 12.0)
};

// Number of pitches in a heptatonic scale.
inline constexpr int kHeptatonicScaleCount = 7;

/// Common musical scale ratios.
inline constexpr std::array<double, kHeptatonicScaleCount> kDiatonicRatios = {
    kSemitoneRatios[1], kSemitoneRatios[3],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[8], kSemitoneRatios[10], kSemitoneRatios[11],
};
inline constexpr std::array<double, kHeptatonicScaleCount> kHarmonicMinorRatios = {
    kSemitoneRatios[1], kSemitoneRatios[2],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[7], kSemitoneRatios[10], kSemitoneRatios[11],
};

}  // namespace

bool Barely_GetNoteFromPitch(BarelyPitchClass pitch_class, int32_t octave, double* out_note) {
  if (out_note == nullptr) return false;
  if (pitch_class < 0 || pitch_class >= static_cast<int>(barely::PitchClass::kCount)) return false;

  static const auto chromatic_scale =
      barely::GetScaleDefinition(barely::ScaleType::kChromatic, kFrequencyA0);
  *out_note = barely::GetNoteFromScale(chromatic_scale,
                                       octave * static_cast<int>(barely::PitchClass::kCount) +
                                           static_cast<int>(pitch_class) -
                                           static_cast<int>(barely::PitchClass::kA));
  return true;
}

bool Barely_GetNoteFromScale(const BarelyScaleDefinition* definition, int32_t degree,
                             double* out_note) {
  if (definition == nullptr) return false;
  if (definition->pitch_ratios == nullptr || definition->pitch_ratio_count == 0) return false;
  if (out_note == nullptr) return false;

  const int note_count = static_cast<int>(definition->pitch_ratio_count);
  const int octave =
      static_cast<int>(std::floor(static_cast<double>(degree) / static_cast<double>(note_count)));
  const int index = degree - octave * note_count;
  assert(index >= 0 && index < note_count);
  *out_note = definition->root_note * std::pow(definition->pitch_ratios[note_count - 1], octave) *
              (index > 0 ? definition->pitch_ratios[index - 1] : 1.0);
  return true;
}

bool Barely_GetScaleDefinition(BarelyScaleType type, double root_note,
                               BarelyScaleDefinition* out_definition) {
  if (type < 0 || type >= static_cast<int>(barely::ScaleType::kCount)) return false;
  if (out_definition == nullptr) return false;

  switch (static_cast<barely::ScaleType>(type)) {
    case barely::ScaleType::kChromatic:
      *out_definition = barely::ScaleDefinition{kSemitoneRatios, root_note};
      return true;
    case barely::ScaleType::kDiatonic:
      *out_definition = barely::ScaleDefinition{kDiatonicRatios, root_note};
      return true;
    case barely::ScaleType::kHarmonicMinor:
      *out_definition = barely::ScaleDefinition{kHarmonicMinorRatios, root_note};
      return true;
    default:
      return false;
  }
}
