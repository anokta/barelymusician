#ifndef EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
#define EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_

#include <windows.h>

#include <unordered_map>

#include "barelymusician/base/event.h"

namespace barelyapi {
namespace examples {

// Windows console input manager for handling keyboard events.
class WinConsoleInput {
 public:
  // Keyboard event callback signatures.
  using Key = char;
  using KeyDownCallback = Event<const Key&>::Callback;
  using KeyUpCallback = Event<const Key&>::Callback;

  WinConsoleInput();

  // Initializes the input manager.
  void Initialize();

  // Shuts down the input manager.
  void Shutdown();

  // Updates the input manager to handle new input events.
  void Update();

  // Registers keyboard key down callback.
  //
  // @param key_down_callback Keyboard key down callback.
  void RegisterKeyDownCallback(KeyDownCallback&& key_down_callback);

  // Registers keyboard key up callback.
  //
  // @param key_up_callback Keyboard key up callback.
  void RegisterKeyUpCallback(KeyUpCallback&& key_up_callback);

 private:
  // Console standard input handle.
  HANDLE std_input_handle_;

  // Old console mode to be restored.
  DWORD previous_console_mode_;

  // Temp input buffer.
  INPUT_RECORD input_buffer_[128];

  // Keyboard key states.
  std::unordered_map<Key, bool> key_states_;

  // Keyboard events.
  Event<const Key&> key_down_event_;
  Event<const Key&> key_up_event_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
