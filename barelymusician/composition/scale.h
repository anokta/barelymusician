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

/// Creates a new scale of type.
///
/// @param type Scale type.
/// @param root_note Root note.
/// @param out_scale Output scale.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_CreateScale(BarelyScaleType type, double root_note,
                                      BarelyScale** out_scale);

/// Gets a note.
///
/// @param pitch_class Pitch class.
/// @param octave Octave.
/// @param out_note Output note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetNote(BarelyPitchClass pitch_class, int32_t octave, double* out_note);

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
  kHarmonicMinor = 1,  /// Harmonic minor scale.
  kMajor = 2,          /// Major scale.
  kNaturalMinor = 3,   /// Natural minor scale.
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
  explicit constexpr ScaleDefinition(std::span<const double> ratios) noexcept
      : ScaleDefinition(BarelyScaleDefinition{ratios.data(), static_cast<int>(ratios.size())}) {}

  /// Constructs a new `ScaleDefinition` from a raw type.
  ///
  /// @param definition Raw scale definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr ScaleDefinition(BarelyScaleDefinition definition) noexcept
      : BarelyScaleDefinition{definition} {
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
  constexpr ScalePtr() noexcept = default;

  /// Creates a new `ScalePtr` from a raw pointer.
  ///
  /// @param scale Raw pointer to scale.
  explicit constexpr ScalePtr(BarelyScale* scale) noexcept : PtrWrapper(scale) {}

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

/// Creates a new scale pointer of type.
///
/// @param type Scale type.
/// @param root_note Root note.
/// @return Scale pointer.
inline ScalePtr CreateScalePtr(ScaleType type, double root_note) noexcept {
  BarelyScale* scale = nullptr;
  [[maybe_unused]] const bool success =
      Barely_CreateScale(static_cast<BarelyScaleType>(type), root_note, &scale);
  return ScalePtr(scale);
}

/// Creates a new scale of type.
///
/// @param type Scale type.
/// @param root_note Root note.
/// @return Scale.
inline Scale CreateScale(ScaleType type, double root_note) noexcept {
  return Scale(CreateScalePtr(type, root_note));
}

/// Returns a note.
///
/// @param pitch_class Pitch class.
/// @param octave Octave.
/// @return Note.
inline double GetNote(PitchClass pitch_class, int octave = 4) noexcept {
  double note = 0.0;
  [[maybe_unused]] const bool success =
      Barely_GetNote(static_cast<BarelyPitchClass>(pitch_class), octave, &note);
  assert(success);
  return note;
}

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
