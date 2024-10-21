#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Scale type enum alias.
typedef int32_t BarelyScaleType;

/// A musical scale.
typedef struct BarelyScale {
  /// Array of pitches relative to the root pitch.
  const double* pitches;

  /// Number of pitches.
  int32_t pitch_count;

  /// Root pitch of the scale.
  double root_pitch;

  /// Mode index.
  int32_t mode;
} BarelyScale;

/// Gets a scale pitch for a given degree.
///
/// @param scale Pointer to scale.
/// @param degree Scale degree.
/// @param out_pitch Output pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree,
                                        double* out_pitch);

/// Gets a scale of type.
///
/// @param type Scale type.
/// @param root_pitch Root pitch.
/// @param out_scale Output scale.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool Barely_GetScale(BarelyScaleType type, double pitch, BarelyScale* out_scale);

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

/// A musical scale.
struct Scale : public BarelyScale {
 public:
  /// Default constructor.
  constexpr Scale() noexcept = default;

  /// Constructs a new `Scale`.
  ///
  /// @param pitches Span of pitches.
  /// @param root_pitch Root pitch.
  /// @param mode Mode.
  constexpr Scale(std::span<const double> pitches, double root_pitch, int mode = 0) noexcept
      : Scale(BarelyScale{pitches.data(), static_cast<int32_t>(pitches.size()), root_pitch,
                          static_cast<int32_t>(mode)}) {}

  /// Constructs a new `Scale` from a raw type.
  ///
  /// @param scale Raw scale.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Scale(BarelyScale scale) noexcept : BarelyScale{scale} {
    assert(scale.pitches != nullptr);
    assert(scale.pitch_count > 0);
    assert(scale.mode >= 0 && scale.mode < scale.pitch_count);
  }

  /// Returns the pitch for a given degree.
  ///
  /// @param degree Degree.
  /// @return Pitch.
  [[nodiscard]] double GetPitch(int degree) const noexcept {
    double pitch = 0.0;
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

/// Returns a scale of type.
///
/// @param type Scale type.
/// @param root_pitch Root pitch.
/// @return Scale.
inline Scale GetScale(ScaleType type, double root_pitch = 0.0) noexcept {
  BarelyScale scale = {};
  [[maybe_unused]] const bool success =
      Barely_GetScale(static_cast<BarelyScaleType>(type), root_pitch, &scale);
  assert(success);
  return scale;
}

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
