#ifndef BARELYMUSICIAN_ENGINE_EVENT_H_
#define BARELYMUSICIAN_ENGINE_EVENT_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps event.
class Event {
 public:
  /// Definition alias.
  using Definition = barely::EventDefinition;

  /// Constructs new `Event`.
  ///
  /// @param definition Event definition.
  explicit Event(const Definition& definition) noexcept;

  /// Destroys `Event`.
  ~Event() noexcept;

  /// Non-copyable and non-movable.
  Event(const Event& other) = delete;
  Event& operator=(const Event& other) = delete;
  Event(Event&& other) noexcept = delete;
  Event& operator=(Event&& other) noexcept = delete;

  /// Processes event.
  void Process() noexcept;

 private:
  // Destroy callback.
  Definition::DestroyCallback destroy_callback_;

  // Process callback.
  Definition::ProcessCallback process_callback_;

  // State.
  void* state_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_EVENT_H_
