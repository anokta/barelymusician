#ifndef BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Low-pass control enum alias.
typedef int32_t BarelyLowPassControl;

/// Low-pass control enum values.
enum BarelyLowPassControl_Values {
  /// Cutoff frequency.
  BarelyLowPassControl_kCutoffFrequency = 0,
};

/// Returns the low-pass effect definition.
///
/// @return Effect definition.
BARELY_EXPORT BarelyEffectDefinition BarelyLowPassEffect_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <array>

#include "barelymusician/dsp/one_pole_filter.h"
#include "barelymusician/effects/custom_effect.h"

namespace barely {

/// Low-pass control.
enum class LowPassControl : int {
  /// Cutoff frequency.
  kCutoffFrequency = BarelyLowPassControl_kCutoffFrequency,
};

/// Simple low-pass effect.
class LowPassEffect : public CustomEffect {
 public:
  /// Constructs new `LowPassEffect`.
  explicit LowPassEffect(int frame_rate) noexcept;

  /// Implements `CustomEffect`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept override;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept override;
  void SetData(const void* /*data*/, int /*size*/) noexcept override {}

  /// Returns the effect definition.
  ///
  /// @return Effect definition.
  static EffectDefinition GetDefinition() noexcept;

 private:
  // Maximum number of output channels allowed.
  static constexpr int kMaxChannelCount = 8;

  // Array of low-pass filter.
  std::array<OnePoleFilter, kMaxChannelCount> filters_;

  // Frame rate in hertz.
  int frame_rate_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
