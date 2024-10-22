#ifndef BARELYMUSICIAN_INTERNAL_EVENT_H_
#define BARELYMUSICIAN_INTERNAL_EVENT_H_

#include <utility>

namespace barely {

/// Template that wraps an event.
template <typename EventType, typename... EventArgs>
class Event {
 public:
  /// Default constructor.
  Event() = default;

  /// Constructs a new `Event`.
  ///
  /// @param event Event.
  Event(const EventType& event) noexcept;

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
  typename EventType::DestroyCallback destroy_callback_ = nullptr;

  // Process callback.
  typename EventType::ProcessCallback process_callback_ = nullptr;

  // State.
  void* state_ = nullptr;
};

template <typename EventType, typename... EventArgs>
Event<EventType, EventArgs...>::Event(Event<EventType, EventArgs...>&& other) noexcept
    : destroy_callback_(std::exchange(other.destroy_callback_, nullptr)),
      process_callback_(std::exchange(other.process_callback_, nullptr)),
      state_(std::exchange(other.state_, nullptr)) {}

template <typename EventType, typename... EventArgs>
Event<EventType, EventArgs...>& Event<EventType, EventArgs...>::operator=(
    Event<EventType, EventArgs...>&& other) noexcept {
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

template <typename EventType, typename... EventArgs>
Event<EventType, EventArgs...>::Event(const EventType& event) noexcept
    : destroy_callback_(event.destroy_callback), process_callback_(event.process_callback) {
  if (event.create_callback) {
    event.create_callback(&state_, event.user_data);
  }
}

template <typename EventType, typename... EventArgs>
Event<EventType, EventArgs...>::~Event() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

template <typename EventType, typename... EventArgs>
void Event<EventType, EventArgs...>::Process(EventArgs... args) noexcept {
  if (process_callback_) {
    process_callback_(&state_, args...);
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_EVENT_H_
