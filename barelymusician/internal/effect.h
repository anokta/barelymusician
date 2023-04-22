#ifndef BARELYMUSICIAN_INTERNAL_EFFECT_H_
#define BARELYMUSICIAN_INTERNAL_EFFECT_H_

#include <cstddef>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps an effect.
class Effect {
 public:
  /// Constructs a new `Effect`.
  ///
  /// @param definition Effect definition.
  /// @param frame_rate Frame rate in hertz.
  Effect(const EffectDefinition& definition, int frame_rate) noexcept;

  /// Destroys `Effect`.
  ~Effect() noexcept;

  /// Non-copyable and non-movable.
  Effect(const Effect& other) noexcept = delete;
  Effect& operator=(const Effect& other) noexcept = delete;
  Effect(Effect&& other) noexcept = delete;
  Effect& operator=(Effect&& other) noexcept = delete;

  /// Processes the next output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count) noexcept;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  /// @return Status.
  void SetControl(int index, double value, double slope_per_frame) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte>& data) noexcept;

 private:
  // Destroy callback.
  const EffectDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const EffectDefinition::ProcessCallback process_callback_;

  // Set control callback.
  const EffectDefinition::SetControlCallback set_control_callback_;

  // Set data callback.
  const EffectDefinition::SetDataCallback set_data_callback_;

  // Data.
  std::vector<std::byte> data_;

  // State.
  void* state_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_EFFECT_H_
