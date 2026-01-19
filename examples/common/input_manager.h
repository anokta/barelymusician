#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_INPUT_MANAGER_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_INPUT_MANAGER_H_

#include <functional>
#include <unordered_set>
#include <utility>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef NOMINMAX
#define NOMINMAX
#endif  // NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif  // defined(__APPLE__)

namespace barely::examples {

/// Input manager for handling keyboard events.
class InputManager {
 public:
  using Key = char;
  using KeyDownCallback = std::function<void(const Key& key)>;
  using KeyUpCallback = std::function<void(const Key& key)>;

  InputManager();
  ~InputManager() noexcept;

  void Update();

  void SetKeyDownCallback(KeyDownCallback key_down_callback) noexcept {
    key_down_callback_ = std::move(key_down_callback);
  }
  void SetKeyUpCallback(KeyUpCallback key_up_callback) noexcept {
    key_up_callback_ = std::move(key_up_callback);
  }

 private:
  void HandleKeyDown(const Key& key);
  void HandleKeyUp(const Key& key) noexcept;

  // Keyboard events.
  KeyDownCallback key_down_callback_;
  KeyUpCallback key_up_callback_;

  std::unordered_set<Key> pressed_keys_;

#if defined(_WIN32) || defined(__CYGWIN__)
  HANDLE std_input_handle_;
  DWORD previous_console_mode_;
#elif defined(__APPLE__)
  using EventCallback = std::function<void(CGEventType type, CGEventRef event)>;

  EventCallback event_callback_;
  CFMachPortRef event_tap_;
  CFRunLoopSourceRef run_loop_source_;
#endif  // defined(__APPLE__)
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_INPUT_MANAGER_H_
