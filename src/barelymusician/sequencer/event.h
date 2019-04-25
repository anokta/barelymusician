#ifndef BARELYAPI_SEQUENCER_EVENT_H_
#define BARELYAPI_SEQUENCER_EVENT_H_

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/module.h"

namespace barelyapi {

// Argument agnostic event that allows a triggering mechanism for all the
// registered callbacks.
template <typename... ArgumentTypes>
class Event : public Module {
 public:
  // Event callback signature.
  using Callback = std::function<void(ArgumentTypes...)>;

  // Implements |Module|.
  void Reset() override;

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
void Event<ArgumentTypes...>::Reset() {
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

#endif  // BARELYAPI_SEQUENCER_EVENT_H_
