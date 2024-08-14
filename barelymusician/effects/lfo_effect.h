#ifndef BARELYMUSICIAN_EFFECTS_LFO_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_LFO_EFFECT_H_

// NOLINTBEGIN
#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Returns the low-frequency oscillator effect definition.
///
/// @return Effect definition.
BARELY_EXPORT BarelyEffectDefinition BarelyLfoEffect_GetDefinition();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus

#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/effects/custom_effect.h"

namespace barely {

/// Simple amplitude low-frequency oscillator effect.
class LfoEffect : public CustomEffect {
 public:
  /// Control enum.
  enum class Control : int {
    /// Oscillator type.
    kOscillatorType = 0,
    /// Oscillator frequency.
    kOscillatorFrequency = 1,
    /// Intensity.
    kIntensity = 2,
    /// Number of controls.
    kCount,
  };

  /// Returns the effect definition.
  ///
  /// @return Effect definition.
  static EffectDefinition GetDefinition() noexcept;

 protected:
  /// Constructs new `LfoEffect`.
  explicit LfoEffect(int frame_rate) noexcept;

  /// Implements `CustomEffect`.
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept final;
  void SetControl(int id, double value) noexcept final;
  void SetData(const void* /*data*/, int /*size*/) noexcept final {}

 private:
  // Low-frequency oscillator.
  Oscillator lfo_;

  // Frequency.
  double frequency_ = 220.0;

  // Intensity.
  double intensity_ = 1.0;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_EFFECTS_LFO_EFFECT_H_
