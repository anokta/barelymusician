#ifndef BARELYMUSICIAN_INTERNAL_EFFECT_H_
#define BARELYMUSICIAN_INTERNAL_EFFECT_H_

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"

namespace barely::internal {

/// Class that wraps an effect.
class Effect {
 public:
  /// Constructs a new `Effect`.
  ///
  /// @param definition Effect definition.
  /// @param frame_rate Frame rate in hertz.
  /// @param process_order Process order.
  Effect(const EffectDefinition& definition, int frame_rate, int process_order) noexcept;

  /// Destroys `Effect`.
  ~Effect() noexcept;

  /// Non-copyable and non-movable.
  Effect(const Effect& other) noexcept = delete;
  Effect& operator=(const Effect& other) noexcept = delete;
  Effect(Effect&& other) noexcept = delete;
  Effect& operator=(Effect&& other) noexcept = delete;

  /// Returns all controls.
  ///
  /// @return Array of controls.
  [[nodiscard]] std::unordered_map<int, Control>& GetAllControls() noexcept;

  /// Returns a control value.
  ///
  /// @param id Control identifier.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] Control* GetControl(int id) noexcept;

  /// Returns the process order.
  ///
  /// @return Process order.
  [[nodiscard]] int GetProcessOrder() const noexcept;

  /// Processes the next output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  void Process(double* output_samples, int output_channel_count, int output_frame_count) noexcept;

  /// Processes a control event.
  ///
  /// @param id Control identifier.
  void ProcessControlEvent(int id) noexcept;

  /// Processes a control message.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  /// @return True if successful, false otherwise.
  void ProcessControlMessage(int id, double value, double slope_per_frame) noexcept;

  /// Processes a data message.
  ///
  /// @param data Data.
  void ProcessDataMessage(std::vector<std::byte>& data) noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept;

  /// Returns the process order.
  ///
  /// @param process_order Process order.
  /// @return True if successful, false otherwise.
  bool SetProcessOrder(int process_order) noexcept;

 private:
  // Destroy callback.
  const EffectDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const EffectDefinition::ProcessCallback process_callback_;

  // Set control callback.
  const EffectDefinition::SetControlCallback set_control_callback_;

  // Set data callback.
  const EffectDefinition::SetDataCallback set_data_callback_;

  // Map of controls by identifiers.
  std::unordered_map<int, Control> controls_;

  // Control event.
  Control::Event control_event_;

  // Process order.
  int process_order_ = 0;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_EFFECT_H_
