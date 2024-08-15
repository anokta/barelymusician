#ifndef BARELYMUSICIAN_INTERNAL_EFFECT_H_
#define BARELYMUSICIAN_INTERNAL_EFFECT_H_

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/message_queue.h"

namespace barely::internal {

/// Class that wraps an effect.
class Effect {
 public:
  /// Constructs a new `Effect`.
  ///
  /// @param definition Effect definition.
  /// @param frame_rate Frame rate in hertz.
  /// @param update_frame Update frame.
  Effect(const EffectDefinition& definition, int frame_rate, int64_t update_frame) noexcept;

  /// Destroys `Effect`.
  ~Effect() noexcept;

  /// Non-copyable and non-movable.
  Effect(const Effect& other) noexcept = delete;
  Effect& operator=(const Effect& other) noexcept = delete;
  Effect(Effect&& other) noexcept = delete;
  Effect& operator=(Effect&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param id Control identifier.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] Control* GetControl(int id) noexcept;
  [[nodiscard]] const Control* GetControl(int id) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param process_frame Process frame.
  /// @return True if successful, false otherwise.
  bool Process(double* output_samples, int output_channel_count, int output_frame_count,
               int64_t process_frame) noexcept;

  /// Resets all control values.
  void ResetAllControls() noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_frame Update frame.
  void Update(int64_t update_frame) noexcept;

 private:
  // Destroy callback.
  const EffectDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const EffectDefinition::ProcessCallback process_callback_;

  // Set control callback.
  const EffectDefinition::SetControlCallback set_control_callback_;

  // Set data callback.
  const EffectDefinition::SetDataCallback set_data_callback_;

  // Control map.
  ControlMap control_map_;

  // Control event.
  Control::Event control_event_;

  // Update frame.
  int64_t update_frame_ = 0;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_EFFECT_H_
