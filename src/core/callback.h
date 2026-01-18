#ifndef BARELYMUSICIAN_CORE_CALLBACK_H_
#define BARELYMUSICIAN_CORE_CALLBACK_H_

namespace barely {

template <typename CallbackType>
struct Callback {
  CallbackType callback = nullptr;
  void* user_data = nullptr;

  constexpr operator bool() const noexcept { return callback; }

  template <typename... CallbackArgs>
  constexpr void operator()(CallbackArgs... args) const noexcept {
    if (callback) {
      callback(args..., user_data);
    }
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CALLBACK_H_
