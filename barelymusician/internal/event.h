#ifndef BARELYMUSICIAN_INTERNAL_EVENT_H_
#define BARELYMUSICIAN_INTERNAL_EVENT_H_

#include <utility>

namespace barely::internal {

/// Template that wraps an event.
template <typename EventDefinitionType, typename... EventArgs>
class Event {
 public:
  /// Default constructor.
  Event() = default;

  /// Constructs a new `Event`.
  ///
  /// @param definition Event definition.
  /// @param user_data Pointer to user data.
  Event(const EventDefinitionType& definition, void* user_data) noexcept;

  /// Destroys `Event`.
  ~Event() noexcept;

  /// Non-copyable.
  Event(const Event& other) noexcept = delete;
  Event& operator=(const Event& other) noexcept = delete;

  /// Constructs new `Event` via move.
  ///
  /// @param other Other event.
  Event(Event&& other) noexcept;

  /// Assigns `Event` via move.
  ///
  /// @param other Other event.
  /// @return Event.
  Event& operator=(Event&& other) noexcept;

  /// Processes the event.
  ///
  /// @param args Process arguments.
  void Process(EventArgs... args) noexcept;

 private:
  // Destroy callback.
  typename EventDefinitionType::DestroyCallback destroy_callback_ = nullptr;

  // Process callback.
  typename EventDefinitionType::ProcessCallback process_callback_ = nullptr;

  // State.
  void* state_ = nullptr;
};

template <typename EventDefinitionType, typename... EventArgs>
Event<EventDefinitionType, EventArgs...>::Event(
    Event<EventDefinitionType, EventArgs...>&& other) noexcept
    : destroy_callback_(std::exchange(other.destroy_callback_, nullptr)),
      process_callback_(std::exchange(other.process_callback_, nullptr)),
      state_(std::exchange(other.state_, nullptr)) {}

template <typename EventDefinitionType, typename... EventArgs>
Event<EventDefinitionType, EventArgs...>&
Event<EventDefinitionType, EventArgs...>::operator=(
    Event<EventDefinitionType, EventArgs...>&& other) noexcept {
  if (this != &other) {
    if (destroy_callback_) {
      destroy_callback_(&state_);
    }
    destroy_callback_ = std::exchange(other.destroy_callback_, nullptr);
    process_callback_ = std::exchange(other.process_callback_, nullptr);
    state_ = std::exchange(other.state_, nullptr);
  }
  return *this;
}

template <typename EventDefinitionType, typename... EventArgs>
Event<EventDefinitionType, EventArgs...>::Event(
    const EventDefinitionType& definition, void* user_data) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback) {
  if (definition.create_callback) {
    definition.create_callback(&state_, user_data);
  }
}

template <typename EventDefinitionType, typename... EventArgs>
Event<EventDefinitionType, EventArgs...>::~Event() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

template <typename EventDefinitionType, typename... EventArgs>
void Event<EventDefinitionType, EventArgs...>::Process(
    EventArgs... args) noexcept {
  if (process_callback_) {
    process_callback_(&state_, args...);
  }
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_EVENT_H_
