#ifndef EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
#define EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_

#include <windows.h>
#include <functional>
#include <unordered_map>

namespace barelyapi {
namespace examples {

// Windows console input manager for handling keyboard events.
class WinConsoleInput {
 public:
  // Keyboard event callback signatures.
  using Key = char;
  using OnKeyDownCallback = std::function<void(const Key&)>;
  using OnKeyUpCallback = std::function<void(const Key&)>;

  WinConsoleInput();

  // Initializes the input manager.
  void Initialize();

  // Shuts down the input manager.
  void Shutdown();

  // Updates the input manager to handle new input events.
  void Update();

  // Sets on keyboard key down callback.
  //
  // @param on_key_down On keyboard key down callback.
  void SetOnKeyDownCallback(OnKeyDownCallback&& on_key_down);

  // Sets on keyboard key up callback.
  //
  // @param on_key_up On keyboard key up callback.
  void SetOnKeyUpCallback(OnKeyUpCallback&& on_key_up);

 private:
  // Console standard input handle.
  HANDLE std_input_handle_;

  // Old console mode to be restored.
  DWORD previous_console_mode_;

  // Temp input buffer.
  INPUT_RECORD input_buffer_[128];

  // Keyboard key states.
  std::unordered_map<Key, bool> key_states_;

  // Keyboard event callbacks.
  OnKeyDownCallback on_key_down_;
  OnKeyUpCallback on_key_up_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
