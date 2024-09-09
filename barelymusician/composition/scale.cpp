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

// Number of pitches in a diatonic scale.
inline constexpr int kDiatonicScaleCount = 7;

/// Common musical scale ratios.
inline constexpr std::array<double, kDiatonicScaleCount> kHarmonicMinorRatios = {
    kSemitoneRatios[1], kSemitoneRatios[2],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[7], kSemitoneRatios[10], kSemitoneRatios[11],
};
inline constexpr std::array<double, kDiatonicScaleCount> kMajorRatios = {
    kSemitoneRatios[1], kSemitoneRatios[3],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[8], kSemitoneRatios[10], kSemitoneRatios[11],
};
inline constexpr std::array<double, kDiatonicScaleCount> kNaturalMinorRatios = {
    kSemitoneRatios[1], kSemitoneRatios[2], kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[7], kSemitoneRatios[9], kSemitoneRatios[11],
};

}  // namespace

// Scale.
struct BarelyScale {
  BarelyScale(const BarelyScaleDefinition& definition, double root_note) noexcept
      : ratios_(definition.ratios, definition.ratios + definition.ratio_count),
        root_note_(root_note) {
    assert(definition.ratio_count == 0 || definition.ratios != nullptr);
  }

  // Returns the corresponding note for a given scale degree.
  double GetNote(int degree) const noexcept {
    if (ratios_.empty()) {
      return 0.0;
    }
    const int note_count = GetNoteCount();
    const int octave =
        static_cast<int>(std::floor(static_cast<double>(degree) / static_cast<double>(note_count)));
    const int index = degree - octave * note_count;
    assert(index >= 0 && index < note_count);
    return root_note_ * std::pow(ratios_[note_count - 1], octave) *
           (index > 0 ? ratios_[index - 1] : 1.0);
  }

  // Returns number of notes.
  int GetNoteCount() const noexcept { return static_cast<int>(ratios_.size()); }

 private:
  // Array of note ratios.
  std::vector<double> ratios_;

  // Root note.
  double root_note_ = 0.0;
};

bool Barely_CreateScale(BarelyScaleType type, double root_note, BarelyScale** out_scale) {
  if (type < 0 || type >= static_cast<int>(barely::ScaleType::kCount)) return false;
  if (out_scale == nullptr) return false;

  barely::ScaleDefinition definition = {};
  switch (static_cast<barely::ScaleType>(type)) {
    case barely::ScaleType::kChromatic:
      definition = barely::ScaleDefinition{kSemitoneRatios};
      break;
    case barely::ScaleType::kHarmonicMinor:
      definition = barely::ScaleDefinition{kHarmonicMinorRatios};
      break;
    case barely::ScaleType::kMajor:
      definition = barely::ScaleDefinition{kMajorRatios};
      break;
    case barely::ScaleType::kNaturalMinor:
      definition = barely::ScaleDefinition{kNaturalMinorRatios};
      break;
    default:
      return false;
  }
  return BarelyScale_Create(definition, root_note, out_scale);
}

bool Barely_GetNote(BarelyPitchClass pitch_class, int32_t octave, double* out_note) {
  if (out_note == nullptr) return false;
  if (pitch_class < 0 || pitch_class >= static_cast<int>(barely::PitchClass::kCount)) return false;

  static const barely::Scale chromatic_scale =
      barely::CreateScale(barely::ScaleType::kChromatic, kFrequencyA0);
  *out_note = chromatic_scale.GetNote(octave * static_cast<int>(barely::PitchClass::kCount) +
                                      static_cast<int>(pitch_class));
  return true;
}

bool BarelyScale_Create(BarelyScaleDefinition definition, double root_note,
                        BarelyScale** out_scale) {
  if (out_scale == nullptr) return false;

  *out_scale = new BarelyScale(definition, root_note);
  return true;
}

bool BarelyScale_Destroy(BarelyScale* scale) {
  if (scale == nullptr) return false;

  delete scale;
  return true;
}

bool BarelyScale_GetNote(const BarelyScale* scale, int32_t degree, double* out_note) {
  if (out_note == nullptr) return false;

  *out_note = scale->GetNote(degree);
  return true;
}

bool BarelyScale_GetNoteCount(const BarelyScale* scale, int32_t* out_note_count) {
  if (out_note_count == nullptr) return false;

  *out_note_count = scale->GetNoteCount();
  return true;
}
