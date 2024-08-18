#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Scale definition.
typedef struct BarelyScaleDefinition {
  /// Array of ratios.
  const double* ratios;

  /// Number of ratios.
  int32_t ratio_count;
} BarelyScaleDefinition;

/// Scale alias.
typedef struct BarelyScale BarelyScale;

/// Creates a new scale.
///
/// @param definition Scale definition.
/// @param root_note Root note.
/// @param out_scale Output scale.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_Create(BarelyScaleDefinition definition, double root_note,
                                      BarelyScale** out_scale);

/// Destroys a scale.
///
/// @param scale Pointer to scale.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_Destroy(BarelyScale* scale);

/// Gets a scale note.
///
/// @param scale Pointer to scale.
/// @param degree Scale degree.
/// @param out_note Output note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_GetNote(const BarelyScale* scale, int32_t degree, double* out_note);

/// Gets the number of notes in a scale.
///
/// @param scale Pointer to scale.
/// @param out_note_count Output number of notes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_GetNoteCount(const BarelyScale* scale, int32_t* out_note_count);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <array>
#include <span>

namespace barely {

/// Scale definition.
struct ScaleDefinition : public BarelyScaleDefinition {
 public:
  /// Default constructor.
  ScaleDefinition() noexcept = default;

  /// Constructs a new `ScaleDefinition`.
  ///
  /// @param ratios Span of ratios.
  explicit ScaleDefinition(std::span<const double> ratios) noexcept
      : BarelyScaleDefinition{ratios.data(), static_cast<int>(ratios.size())} {}

  /// Constructs a new `ScaleDefinition` from a raw type.
  ///
  /// @param definition Raw scale definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ScaleDefinition(BarelyScaleDefinition definition) noexcept : BarelyScaleDefinition{definition} {
    assert(definition.ratios != nullptr);
    assert(definition.ratio_count > 0);
  }
};

/// Class that wraps a scale pointer.
class ScalePtr : public PtrWrapper<BarelyScale> {
 public:
  /// Creates a new `ScalePtr`.
  ///
  /// @param definition Scale definition.
  /// @param root_note Root note.
  /// @return Scale pointer.
  [[nodiscard]] static ScalePtr Create(ScaleDefinition definition, double root_note) noexcept {
    BarelyScale* scale;
    [[maybe_unused]] const bool success = BarelyScale_Create(definition, root_note, &scale);
    assert(success);
    return ScalePtr(scale);
  }

  /// Destroys a `ScalePtr`.
  ///
  /// @param scale Scale pointer.
  static void Destroy(ScalePtr scale) noexcept { BarelyScale_Destroy(scale); }

  /// Default constructor.
  ScalePtr() noexcept = default;

  /// Creates a new `ScalePtr` from a raw pointer.
  ///
  /// @param scale Raw pointer to scale.
  explicit ScalePtr(BarelyScale* scale) noexcept : PtrWrapper(scale) {}

  /// Returns note.
  ///
  /// @param degree Degree.
  /// @return Note.
  [[nodiscard]] double GetNote(int degree) const noexcept {
    double note = 0.0;
    [[maybe_unused]] const bool success = BarelyScale_GetNote(*this, degree, &note);
    assert(success);
    return note;
  }

  /// Returns note.
  ///
  /// @param octave Scale octave.
  /// @param degree Scale degree.
  /// @return Note.
  [[nodiscard]] double GetNote(int octave, int degree) const noexcept {
    return GetNote(octave * GetNoteCount() + degree);
  }

  /// Returns the number of notes.
  ///
  /// @return Number of notes.
  [[nodiscard]] int GetNoteCount() const noexcept {
    int32_t note_count = 0;
    [[maybe_unused]] const bool success = BarelyScale_GetNoteCount(*this, &note_count);
    assert(success);
    return note_count;
  }
};

/// Scoped scale alias.
using Scale = ScopedWrapper<ScalePtr>;

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr double kSemitoneCount = 12.0;

/// Semitone pitch intervals of an octave.
inline constexpr std::array<double, static_cast<int>(kSemitoneCount)> kSemitoneRatios = {
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

/// Common musical scale ratios.
inline constexpr std::array<double, 7> kRatiosMajorScale = {
    kSemitoneRatios[1], kSemitoneRatios[3],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[8], kSemitoneRatios[10], kSemitoneRatios[11],
};
inline constexpr std::array<double, 7> kRatiosNaturalMinorScale = {
    kSemitoneRatios[1], kSemitoneRatios[2], kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[7], kSemitoneRatios[9], kSemitoneRatios[11],
};
inline constexpr std::array<double, 7> kRatiosHarmonicMinorScale = {
    kSemitoneRatios[1], kSemitoneRatios[2],  kSemitoneRatios[4],  kSemitoneRatios[6],
    kSemitoneRatios[7], kSemitoneRatios[10], kSemitoneRatios[11],
};

// A0 reference frequency.
inline constexpr double kFrequencyA0 = 27.5;

/// Common note values.
inline constexpr double kNoteA0 = kFrequencyA0;
inline constexpr double kNoteAsharp0 = kNoteA0 * kSemitoneRatios[0];
inline constexpr double kNoteB0 = kNoteA0 * kSemitoneRatios[1];
inline constexpr double kNoteC1 = kNoteA0 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp1 = kNoteA0 * kSemitoneRatios[3];
inline constexpr double kNoteD1 = kNoteA0 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp1 = kNoteA0 * kSemitoneRatios[5];
inline constexpr double kNoteE1 = kNoteA0 * kSemitoneRatios[6];
inline constexpr double kNoteF1 = kNoteA0 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp1 = kNoteA0 * kSemitoneRatios[8];
inline constexpr double kNoteG1 = kNoteA0 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp1 = kNoteA0 * kSemitoneRatios[10];
inline constexpr double kNoteA1 = kNoteA0 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp1 = kNoteA1 * kSemitoneRatios[0];
inline constexpr double kNoteB1 = kNoteA1 * kSemitoneRatios[1];
inline constexpr double kNoteC2 = kNoteA1 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp2 = kNoteA1 * kSemitoneRatios[3];
inline constexpr double kNoteD2 = kNoteA1 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp2 = kNoteA1 * kSemitoneRatios[5];
inline constexpr double kNoteE2 = kNoteA1 * kSemitoneRatios[6];
inline constexpr double kNoteF2 = kNoteA1 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp2 = kNoteA1 * kSemitoneRatios[8];
inline constexpr double kNoteG2 = kNoteA1 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp2 = kNoteA1 * kSemitoneRatios[10];
inline constexpr double kNoteA2 = kNoteA1 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp2 = kNoteA2 * kSemitoneRatios[0];
inline constexpr double kNoteB2 = kNoteA2 * kSemitoneRatios[1];
inline constexpr double kNoteC3 = kNoteA2 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp3 = kNoteA2 * kSemitoneRatios[3];
inline constexpr double kNoteD3 = kNoteA2 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp3 = kNoteA2 * kSemitoneRatios[5];
inline constexpr double kNoteE3 = kNoteA2 * kSemitoneRatios[6];
inline constexpr double kNoteF3 = kNoteA2 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp3 = kNoteA2 * kSemitoneRatios[8];
inline constexpr double kNoteG3 = kNoteA2 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp3 = kNoteA2 * kSemitoneRatios[10];
inline constexpr double kNoteA3 = kNoteA2 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp3 = kNoteA3 * kSemitoneRatios[0];
inline constexpr double kNoteB3 = kNoteA3 * kSemitoneRatios[1];
inline constexpr double kNoteC4 = kNoteA3 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp4 = kNoteA3 * kSemitoneRatios[3];
inline constexpr double kNoteD4 = kNoteA3 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp4 = kNoteA3 * kSemitoneRatios[5];
inline constexpr double kNoteE4 = kNoteA3 * kSemitoneRatios[6];
inline constexpr double kNoteF4 = kNoteA3 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp4 = kNoteA3 * kSemitoneRatios[8];
inline constexpr double kNoteG4 = kNoteA3 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp4 = kNoteA3 * kSemitoneRatios[10];
inline constexpr double kNoteA4 = kNoteA3 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp4 = kNoteA4 * kSemitoneRatios[0];
inline constexpr double kNoteB4 = kNoteA4 * kSemitoneRatios[1];
inline constexpr double kNoteC5 = kNoteA4 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp5 = kNoteA4 * kSemitoneRatios[3];
inline constexpr double kNoteD5 = kNoteA4 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp5 = kNoteA4 * kSemitoneRatios[5];
inline constexpr double kNoteE5 = kNoteA4 * kSemitoneRatios[6];
inline constexpr double kNoteF5 = kNoteA4 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp5 = kNoteA4 * kSemitoneRatios[8];
inline constexpr double kNoteG5 = kNoteA4 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp5 = kNoteA4 * kSemitoneRatios[10];
inline constexpr double kNoteA5 = kNoteA4 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp5 = kNoteA5 * kSemitoneRatios[0];
inline constexpr double kNoteB5 = kNoteA5 * kSemitoneRatios[1];
inline constexpr double kNoteC6 = kNoteA5 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp6 = kNoteA5 * kSemitoneRatios[3];
inline constexpr double kNoteD6 = kNoteA5 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp6 = kNoteA5 * kSemitoneRatios[5];
inline constexpr double kNoteE6 = kNoteA5 * kSemitoneRatios[6];
inline constexpr double kNoteF6 = kNoteA5 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp6 = kNoteA5 * kSemitoneRatios[8];
inline constexpr double kNoteG6 = kNoteA5 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp6 = kNoteA5 * kSemitoneRatios[10];
inline constexpr double kNoteA6 = kNoteA5 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp6 = kNoteA6 * kSemitoneRatios[0];
inline constexpr double kNoteB6 = kNoteA6 * kSemitoneRatios[1];
inline constexpr double kNoteC7 = kNoteA6 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp7 = kNoteA6 * kSemitoneRatios[3];
inline constexpr double kNoteD7 = kNoteA6 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp7 = kNoteA6 * kSemitoneRatios[5];
inline constexpr double kNoteE7 = kNoteA6 * kSemitoneRatios[6];
inline constexpr double kNoteF7 = kNoteA6 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp7 = kNoteA6 * kSemitoneRatios[8];
inline constexpr double kNoteG7 = kNoteA6 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp7 = kNoteA6 * kSemitoneRatios[10];
inline constexpr double kNoteA7 = kNoteA6 * kSemitoneRatios[11];
inline constexpr double kNoteAsharp7 = kNoteA7 * kSemitoneRatios[0];
inline constexpr double kNoteB7 = kNoteA7 * kSemitoneRatios[1];
inline constexpr double kNoteC8 = kNoteA7 * kSemitoneRatios[2];
inline constexpr double kNoteCsharp8 = kNoteA7 * kSemitoneRatios[3];
inline constexpr double kNoteD8 = kNoteA7 * kSemitoneRatios[4];
inline constexpr double kNoteDsharp8 = kNoteA7 * kSemitoneRatios[5];
inline constexpr double kNoteE8 = kNoteA7 * kSemitoneRatios[6];
inline constexpr double kNoteF8 = kNoteA7 * kSemitoneRatios[7];
inline constexpr double kNoteFsharp8 = kNoteA7 * kSemitoneRatios[8];
inline constexpr double kNoteG8 = kNoteA7 * kSemitoneRatios[9];
inline constexpr double kNoteGsharp8 = kNoteA7 * kSemitoneRatios[10];
inline constexpr double kNoteA8 = kNoteA7 * kSemitoneRatios[11];
inline constexpr double kNoteKick = kNoteC3;
inline constexpr double kNoteSnare = kNoteD3;
inline constexpr double kNoteHihatClosed = kNoteE3;
inline constexpr double kNoteHihatOpen = kNoteF3;

/// Middle note values.
inline constexpr double kNoteC = kNoteC4;
inline constexpr double kNoteCsharp = kNoteCsharp4;
inline constexpr double kNoteD = kNoteD4;
inline constexpr double kNoteDsharp = kNoteDsharp4;
inline constexpr double kNoteE = kNoteE4;
inline constexpr double kNoteF = kNoteF4;
inline constexpr double kNoteFsharp = kNoteFsharp4;
inline constexpr double kNoteG = kNoteG4;
inline constexpr double kNoteGsharp = kNoteGsharp4;
inline constexpr double kNoteA = kNoteA4;
inline constexpr double kNoteASharp = kNoteAsharp4;
inline constexpr double kNoteB = kNoteB4;

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
