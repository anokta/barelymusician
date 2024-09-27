#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Scale type enum alias.
typedef int32_t BarelyScaleType;

/// Definition of a musical scale.
#pragma pack(push, 1)
typedef struct BarelyScaleDefinition {
  /// Array of pitches relative to the root pitch.
  const int32_t* pitches;

  /// Number of pitches.
  int32_t pitch_count;

  /// Root pitch of the scale.
  int32_t root_pitch;

  /// Mode index.
  int32_t mode;
} BarelyScaleDefinition;
#pragma pack(pop)

/// Gets a scale pitch for a given degree.
///
/// @param definition Pointer to scale definition.
/// @param degree Scale degree.
/// @param out_pitch Output pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_GetPitch(const BarelyScaleDefinition* definition, int32_t degree,
                                        int32_t* out_pitch);

/// Gets a scale definition of type.
///
/// @param type Scale type.
/// @param root_pitch Root pitch.
/// @param out_definition Output scale definition.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetScaleDefinition(BarelyScaleType type, int32_t pitch,
                                             BarelyScaleDefinition* out_definition);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <array>
#include <span>

namespace barely {

/// Scale type enum.
enum class ScaleType : BarelyScaleType {
  kChromatic = 0,      /// Chromatic scale.
  kDiatonic = 1,       /// Diatonic scale.
  kHarmonicMinor = 2,  /// Harmonic minor scale.
  kCount,
};

/// Definition of a musical scale.
struct ScaleDefinition : public BarelyScaleDefinition {
 public:
  /// Default constructor.
  constexpr ScaleDefinition() noexcept = default;

  /// Constructs a new `ScaleDefinition`.
  ///
  /// @param pitches Span of pitches.
  /// @param root_pitch Root pitch.
  /// @param mode Mode.
  constexpr ScaleDefinition(std::span<const int> pitches, int root_pitch, int mode = 0) noexcept
      : ScaleDefinition(BarelyScaleDefinition{
            reinterpret_cast<const int32_t*>(pitches.data()), static_cast<int32_t>(pitches.size()),
            static_cast<int32_t>(root_pitch), static_cast<int32_t>(mode)}) {}

  /// Constructs a new `ScaleDefinition` from a raw type.
  ///
  /// @param definition Raw scale definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr ScaleDefinition(BarelyScaleDefinition definition) noexcept
      : BarelyScaleDefinition{definition} {
    assert(definition.pitches != nullptr);
    assert(definition.pitch_count > 0);
    assert(definition.mode >= 0 && definition.mode < definition.pitch_count);
  }

  /// Returns the pitch for a given degree.
  ///
  /// @param definition Scale definition.
  /// @param degree Degree.
  /// @return Pitch.
  [[nodiscard]] int GetPitch(int degree) const noexcept {
    int32_t pitch = 0;
    [[maybe_unused]] const bool success = BarelyScale_GetPitch(this, degree, &pitch);
    assert(success);
    return pitch;
  }

  /// Returns the number of pitches in the scale.
  ///
  /// @return Number of pitches.
  [[nodiscard]] constexpr int GetPitchCount() const noexcept {
    return static_cast<int>(pitch_count);
  }
};

/// Returns a scale definition of type.
///
/// @param type Scale type.
/// @param root_pitch Root pitch.
/// @return Scale definition.
inline ScaleDefinition GetScaleDefinition(ScaleType type, int root_pitch) noexcept {
  BarelyScaleDefinition definition = {};
  [[maybe_unused]] const bool success = Barely_GetScaleDefinition(
      static_cast<BarelyScaleType>(type), static_cast<int32_t>(root_pitch), &definition);
  assert(success);
  return definition;
}

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
