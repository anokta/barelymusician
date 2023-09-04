#ifndef BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_

#include <array>

#include "barelymusician/common/custom_macros.h"
#include "barelymusician/dsp/one_pole_filter.h"
#include "barelymusician/effects/custom_effect.h"

namespace barely {

/// Low-pass effect definition.
#define BARELY_LOW_PASS_EFFECT_CONTROLS(LowPassEffectControl, X) \
  /* Cutoff frequency. */                                        \
  X(LowPassEffectControl, CutoffFrequency, 48000.0, 0.0)
BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(LowPassEffect,
                                         BARELY_LOW_PASS_EFFECT_CONTROLS)

/// Simple low-pass effect.
class LowPassEffect : public CustomEffect {
 public:
  /// Constructs new `LowPassEffect`.
  explicit LowPassEffect(int frame_rate) noexcept;

  /// Implements `CustomEffect`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value,
                  double slope_per_frame) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}

  // Maximum number of output channels allowed.
  static constexpr int kMaxChannelCount = 8;

  // Array of low-pass filter.
  std::array<OnePoleFilter, kMaxChannelCount> filters_;

  // Frame rate in hertz.
  int frame_rate_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_EFFECTS_LOW_PASS_EFFECT_H_
