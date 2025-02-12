#ifndef BARELYMUSICIAN_INTERNAL_CALLBACK_H_
#define BARELYMUSICIAN_INTERNAL_CALLBACK_H_

namespace barely::internal {

/// Callback template.
template <typename CallbackType>
struct Callback {
  /// Callback.
  CallbackType callback = nullptr;

  /// Pointer to user data.
  void* user_data = nullptr;

  /// Returns whether the callback is valid or not.
  ///
  /// @return True if valid, false otherwise.
  constexpr operator bool() const noexcept { return callback; }

  /// Calls the callback.
  ///
  /// @param args Callback arguments.
  template <typename... CallbackArgs>
  constexpr void operator()(CallbackArgs... args) noexcept {
    if (callback) {
      callback(args..., user_data);
    }
  }
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_CALLBACK_H_
