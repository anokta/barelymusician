#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Pitch class enum alias.
typedef int32_t BarelyPitchClass;

/// Scale type enum alias.
typedef int32_t BarelyScaleType;

/// Definition of a musical scale.
#pragma pack(push, 1)
typedef struct BarelyScaleDefinition {
  /// Array of pitch ratios relative to, but excluding, the root note.
  const double* pitch_ratios;

  /// Number of pitch ratios.
  int32_t pitch_ratio_count;

  /// Root note.
  double root_note;
} BarelyScaleDefinition;
#pragma pack(pop)

/// Gets a note from a given pitch class and octave.
///
/// @param pitch_class Pitch class.
/// @param octave Octave.
/// @param out_note Output note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetNoteFromPitch(BarelyPitchClass pitch_class, int32_t octave,
                                           double* out_note);

/// Gets a note from a scale based on a given degree.
///
/// @param definition Pointer to scale definition.
/// @param degree Scale degree.
/// @param out_note Output note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetNoteFromScale(const BarelyScaleDefinition* definition, int32_t degree,
                                           double* out_note);

/// Gets a scale definition of type.
///
/// @param type Scale type.
/// @param root_note Root note.
/// @param out_definition Output scale definition.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetScaleDefinition(BarelyScaleType type, double root_note,
                                             BarelyScaleDefinition* out_definition);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <array>
#include <span>

namespace barely {

/// Pitch class enum.
enum class PitchClass : BarelyPitchClass {
  kC = 0,        /// C
  kCsharp = 1,   /// C#
  kD = 2,        /// D
  kDsharp = 3,   /// D#
  kE = 4,        /// E
  kF = 5,        /// F
  kFsharp = 6,   /// F#
  kG = 7,        /// G
  kGsharp = 8,   /// G#
  kA = 9,        /// A
  kAsharp = 10,  /// A#
  kB = 11,       /// B
  kCount,
  kKick = kC,
  kSnare = kD,
  kHihatClosed = kE,
  kHihatOpen = kF,
};

/// Scale type enum.
enum class ScaleType : BarelyScaleType {
  kChromatic = 0,      /// Chromatic scale.
  kDiatonic = 1,       /// Diatonic scale.
  kHarmonicMinor = 2,  /// Harmonic minor scale.
  kCount,
};

/// Scale definition.
struct ScaleDefinition : public BarelyScaleDefinition {
 public:
  /// Default constructor.
  constexpr ScaleDefinition() noexcept = default;

  /// Constructs a new `ScaleDefinition`.
  ///
  /// @param ratios Span of ratios.
  /// @param root_note Root note.
  constexpr ScaleDefinition(std::span<const double> pitch_ratios, double root_note) noexcept
      : ScaleDefinition(BarelyScaleDefinition{pitch_ratios.data(),
                                              static_cast<int>(pitch_ratios.size()), root_note}) {}

  /// Constructs a new `ScaleDefinition` from a raw type.
  ///
  /// @param definition Raw scale definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr ScaleDefinition(BarelyScaleDefinition definition) noexcept
      : BarelyScaleDefinition{definition} {
    assert(definition.pitch_ratios != nullptr);
    assert(definition.pitch_ratio_count > 0);
  }

  /// Returns the number of notes in the scale.
  ///
  /// @return Number of notes.
  [[nodiscard]] constexpr int GetNoteCount() const noexcept {
    return static_cast<int>(pitch_ratio_count);
  }
};

/// Returns a note from a given pitch class and octave.
///
/// @param pitch_class Pitch class.
/// @param octave Octave.
/// @return Note.
[[nodiscard]] inline double GetNoteFromPitch(PitchClass pitch_class, int octave = 4) noexcept {
  double note = 0.0;
  [[maybe_unused]] const bool success =
      Barely_GetNoteFromPitch(static_cast<BarelyPitchClass>(pitch_class), octave, &note);
  assert(success);
  return note;
}

/// Returns a note from a scale based on a given degree.
///
/// @param definition Scale definition.
/// @param degree Degree.
/// @return Note.
[[nodiscard]] inline double GetNoteFromScale(const ScaleDefinition& definition,
                                             int degree) noexcept {
  double note = 0.0;
  [[maybe_unused]] const bool success = Barely_GetNoteFromScale(&definition, degree, &note);
  assert(success);
  return note;
}

/// Returns a scale definition of type.
///
/// @param type Scale type.
/// @param root_note Root note.
/// @return Scale definition.
inline ScaleDefinition GetScaleDefinition(ScaleType type, double root_note) noexcept {
  BarelyScaleDefinition definition = {};
  [[maybe_unused]] const bool success =
      Barely_GetScaleDefinition(static_cast<BarelyScaleType>(type), root_note, &definition);
  assert(success);
  return definition;
}

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
