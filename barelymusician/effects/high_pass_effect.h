#ifndef BARELYMUSICIAN_EFFECTS_HIGH_PASS_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_HIGH_PASS_EFFECT_H_

#include <array>

#include "barelymusician/common/custom_macros.h"
#include "barelymusician/dsp/one_pole_filter.h"
#include "barelymusician/effects/custom_effect.h"

namespace barely {

/// High-pass effect definition.
#define BARELY_HIGH_PASS_EFFECT_CONTROLS(HighPassEffectControl, X) \
  /* Cutoff frequency. */                                          \
  X(HighPassEffectControl, CutoffFrequency, 0.0, 0.0)
BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(HighPassEffect,
                                         BARELY_HIGH_PASS_EFFECT_CONTROLS)

/// Simple high-pass effect.
class HighPassEffect : public CustomEffect {
 public:
  /// Constructs new `HighPassEffect`.
  explicit HighPassEffect(int frame_rate) noexcept;

  /// Implements `CustomEffect`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}

  // Maximum number of output channels alhighed.
  static constexpr int kMaxChannelCount = 8;

  // Array of high-pass filter.
  std::array<OnePoleFilter, kMaxChannelCount> filters_;

  // Frame rate in hertz.
  int frame_rate_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_EFFECTS_HIGH_PASS_EFFECT_H_
