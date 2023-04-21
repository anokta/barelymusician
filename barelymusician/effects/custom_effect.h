#ifndef BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_

#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Custom effect interface.
class CustomEffect {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~CustomEffect() = default;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  virtual void Process(double* output_samples, int output_channel_count,
                       int output_frame_count) noexcept = 0;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetControl(int index, double value,
                          double slope_per_frame) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;
};

/// Returns the effect definition for `CustomEffectType`.
template <typename CustomEffectType>
EffectDefinition GetEffectDefinition(
    const std::vector<ControlDefinition>& control_definitions) noexcept {
  return EffectDefinition(
      [](void** state, int frame_rate) noexcept {
        // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
        *state = static_cast<void*>(new CustomEffectType(frame_rate));
      },
      [](void** state) noexcept {
        delete static_cast<CustomEffectType*>(*state);
        *state = nullptr;
      },
      [](void** state, double* output_samples, int output_channel_count,
         int output_frame_count) noexcept {
        auto* effect = static_cast<CustomEffectType*>(*state);
        effect->Process(output_samples, output_channel_count,
                        output_frame_count);
      },
      [](void** state, int index, double value, double slope_per_frame) {
        auto* effect = static_cast<CustomEffectType*>(*state);
        effect->SetControl(index, value, slope_per_frame);
      },
      [](void** state, const void* data, int size) noexcept {
        auto* effect = static_cast<CustomEffectType*>(*state);
        effect->SetData(data, size);
      },
      control_definitions);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_