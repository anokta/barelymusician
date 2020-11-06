#ifndef EXAMPLES_UTIL_INPUT_MANAGER_H_
#define EXAMPLES_UTIL_INPUT_MANAGER_H_

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif  // defined(__APPLE__)

#include <functional>
#include <unordered_set>

namespace barelyapi {
namespace examples {

// Input manager for handling keyboard events.
class InputManager {
 public:
  // Keyboard event callback signatures.
  using Key = char;
  using KeyDownCallback = std::function<void(const Key& key)>;
  using KeyUpCallback = std::function<void(const Key& key)>;

  // Constructs new |InputManager|.
  InputManager();

  // Destructs |InputManager|.
  ~InputManager();

  // Sets keyboard key down callback.
  //
  // @param key_down_callback Keyboard key down callback.
  void SetKeyDownCallback(KeyDownCallback key_down_callback);

  // Sets keyboard key up callback.
  //
  // @param key_up_callback Keyboard key up callback.
  void SetKeyUpCallback(KeyUpCallback key_up_callback);

  // Updates the input manager to handle new input events.
  void Update();

 private:
  // Handles keyboard key down event.
  void HandleKeyDown(const Key& key);

  // Handles keyboard key up event.
  void HandleKeyUp(const Key& key);

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

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_INPUT_MANAGER_H_
