#ifndef BARELYAPI_BASE_EVENT_H_
#define BARELYAPI_BASE_EVENT_H_

#include <functional>
#include <utility>
#include <vector>

namespace barelyapi {

// Argument agnostic event that allows a triggering mechanism for all the
// registered callbacks.
// TODO(#22): Implement |Deregister| to support more flexible registration.
template <typename... ArgumentTypes>
class Event {
 public:
  // Event callback signature.
  using Callback = std::function<void(ArgumentTypes...)>;

  // Clears all callbacks.
  void Clear();

  // Registers callback to the event.
  //
  // @param callback Event callback.
  void Register(Callback&& callback);

  // Triggers the event for all the registered callbacks.
  //
  // @param arguments List of arguments to trigger the callbacks with.
  void Trigger(ArgumentTypes... arguments) const;

 private:
  // List of callbacks.
  std::vector<Callback> callbacks_;
};

template <typename... ArgumentTypes>
void Event<ArgumentTypes...>::Clear() {
  callbacks_.clear();
}

template <typename... ArgumentTypes>
void Event<ArgumentTypes...>::Register(Callback&& callback) {
  callbacks_.push_back(std::move(callback));
}

template <typename... ArgumentTypes>
void Event<ArgumentTypes...>::Trigger(ArgumentTypes... arguments) const {
  for (const auto& callback : callbacks_) {
    callback(arguments...);
  }
}

}  // namespace barelyapi

#endif  // BARELYAPI_BASE_EVENT_H_
