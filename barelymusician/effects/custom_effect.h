#ifndef BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_
#define BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
#include <cassert>
#include <cstdint>
#include <new>
#include <span>

namespace barely {

/// Custom effect interface.
class CustomEffect {
 protected:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~CustomEffect() = default;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  virtual void Process(float* output_samples, int output_channel_count,
                       int output_frame_count) noexcept = 0;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetControl(int index, Rational value, Rational slope_per_frame) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;

  /// Returns the effect definition for `CustomEffectType`.
  ///
  /// @param control_definitions Array of control definitions.
  /// @return Effect definition.
  template <typename CustomEffectType>
  static EffectDefinition GetDefinition(
      std::span<const ControlDefinition> control_definitions) noexcept {
    class PublicEffect : public CustomEffectType {
     public:
      explicit PublicEffect(int frame_rate) : CustomEffectType(frame_rate) {}
      using CustomEffectType::Process;
      using CustomEffectType::SetControl;
      using CustomEffectType::SetData;
    };
    return EffectDefinition(
        [](void** state, int32_t frame_rate) noexcept {
          *state = static_cast<void*>(new (std::nothrow) PublicEffect(frame_rate));
          assert(*state);
        },
        [](void** state) noexcept {
          delete static_cast<PublicEffect*>(*state);
          *state = nullptr;
        },
        [](void** state, float* output_samples, int32_t output_channel_count,
           int32_t output_frame_count) noexcept {
          auto* effect = static_cast<PublicEffect*>(*state);
          effect->Process(output_samples, output_channel_count, output_frame_count);
        },
        [](void** state, int32_t index, BarelyRational value, BarelyRational slope_per_frame) {
          auto* effect = static_cast<PublicEffect*>(*state);
          effect->SetControl(index, value, slope_per_frame);
        },
        [](void** state, const void* data, int32_t size) noexcept {
          auto* effect = static_cast<PublicEffect*>(*state);
          effect->SetData(data, size);
        },
        control_definitions);
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_EFFECTS_CUSTOM_EFFECT_H_
