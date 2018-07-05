#ifndef EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
#define EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_

#include <functional>
#include <unordered_map>
#include <windows.h>

// Windows console input manager for handling keyboard events.
class WinConsoleInput {
 public:
  // Alias for the keyboard event callback functions.
  typedef char Key;
  typedef std::function<void(const Key&)> OnKeyDownCallback;
  typedef std::function<void(const Key&)> OnKeyUpCallback;

  WinConsoleInput();

  // Initializes the input manager.
  void Initialize();

  // Updates the input manager to handle new input events.
  void Update();

  // Shuts down the input manager.
  void Shutdown();

  // Sets on keyboard key down callback.
  //
  // @param on_key_down On keyboard key down callback function.
  void SetOnKeyDownCallback(OnKeyDownCallback&& on_key_down);

  // Sets on keyboard key up callback.
  //
  // @param on_key_up On keyboard key up callback function.
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

   // Keyboard event callback functions.
   OnKeyDownCallback on_key_down_;
   OnKeyUpCallback on_key_up_;
};

#endif  // EXAMPLES_UTIL_INPUT_MANAGER_WIN_CONSOLE_INPUT_H_
