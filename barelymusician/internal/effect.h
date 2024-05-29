#ifndef BARELYMUSICIAN_INTERNAL_EFFECT_H_
#define BARELYMUSICIAN_INTERNAL_EFFECT_H_

#include <cstddef>
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
  /// @param initial_timestamp Initial timestamp in seconds.
  Effect(const EffectDefinition& definition, int frame_rate, double initial_timestamp) noexcept;

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

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool Process(double* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept;

  /// Resets all control values.
  void ResetAllControls() noexcept;

  /// Resets a control value.
  ///
  /// @param id Control identifier.
  /// @return True if successful, false otherwise.
  bool ResetControl(int id) noexcept;

  /// Sets a control value.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  /// @return True if successful, false otherwise.
  bool SetControl(int id, double value) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Updates the instrument.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Destroy callback.
  const EffectDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const EffectDefinition::ProcessCallback process_callback_;

  // Set control callback.
  const EffectDefinition::SetControlCallback set_control_callback_;

  // Set data callback.
  const EffectDefinition::SetDataCallback set_data_callback_;

  // Frame rate in hertz.
  const int frame_rate_;

  // Map of controls by identifiers.
  std::unordered_map<int, Control> controls_;

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
