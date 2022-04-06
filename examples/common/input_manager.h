#ifndef EXAMPLES_COMMON_INPUT_MANAGER_H_
#define EXAMPLES_COMMON_INPUT_MANAGER_H_

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif  // defined(__APPLE__)

#include <functional>
#include <unordered_set>

namespace barely::examples {

/// Input manager for handling keyboard events.
class InputManager {
 public:
  /// Keyboard key signature.
  using Key = char;

  /// Keyboard key down callback signature.
  ///
  /// @param key Keyboard key.
  using KeyDownCallback = std::function<void(const Key& key)>;

  /// Keyboard key up callback signature.
  ///
  /// @param key Keyboard key.
  using KeyUpCallback = std::function<void(const Key& key)>;

  /// Constructs new `InputManager`.
  InputManager() noexcept;

  /// Destructs `InputManager`.
  ~InputManager() noexcept;

  /// Sets keyboard key down callback.
  ///
  /// @param key_down_callback Keyboard key down callback.
  void SetKeyDownCallback(KeyDownCallback key_down_callback) noexcept;

  /// Sets keyboard key up callback.
  ///
  /// @param key_up_callback Keyboard key up callback.
  void SetKeyUpCallback(KeyUpCallback key_up_callback) noexcept;

  /// Updates the input manager to handle new input events.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update() noexcept;

 private:
  // Handles keyboard key down event.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void HandleKeyDown(const Key& key) noexcept;

  // Handles keyboard key up event.
  void HandleKeyUp(const Key& key) noexcept;

  // Keyboard events.
  KeyDownCallback key_down_callback_;
  KeyUpCallback key_up_callback_;

  // Currently pressed keyboard keys.
  std::unordered_set<Key> pressed_keys_;

#if defined(_WIN32) || defined(__CYGWIN__)
  // Console standard input handle.
  HANDLE std_input_handle_;

  // Old console mode to be restored.
  DWORD previous_console_mode_;

  // Temp input buffer.
  INPUT_RECORD input_buffer_[128];
#elif defined(__APPLE__)
  // Keyboard event callback signature.
  using EventCallback = std::function<void(CGEventType type, CGEventRef event)>;

  // Keyboard event callback.
  EventCallback event_callback_;

  // Keyboard event tap.
  CFMachPortRef event_tap_;

  // Keyboard event run loop source.
  CFRunLoopSourceRef run_loop_source_;
#endif  // defined(__APPLE__)
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_INPUT_MANAGER_H_
