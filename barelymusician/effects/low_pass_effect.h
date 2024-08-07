#ifndef BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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

/// Simple low-pass effect.
class LowPassEffect : public CustomEffect {
 public:
  /// Control enum.
  enum class Control : int {
    /// Cutoff frequency.
    kCutoffFrequency = 0,
    /// Number of controls.
    kCount,
  };

  /// Returns the effect definition.
  ///
  /// @return Effect definition.
  static EffectDefinition GetDefinition() noexcept;

 protected:
  /// Constructs new `LowPassEffect`.
  explicit LowPassEffect(int frame_rate) noexcept;

  /// Implements `CustomEffect`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  void SetControl(int control_id, double value) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}

 private:
  // Maximum number of output channels allowed.
  static constexpr int kMaxChannelCount = 8;

  // Cutoff frequency.
  double cutoff_frequency_ = 0.0;

  // Array of low-pass filter.
  std::array<OnePoleFilter, kMaxChannelCount> filters_;

  // Frame rate in hertz.
  int frame_rate_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
