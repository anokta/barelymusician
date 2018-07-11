#include "util/input_manager/win_console_input.h"

namespace barelyapi {
namespace examples {

WinConsoleInput::WinConsoleInput()
    : std_input_handle_(nullptr), previous_console_mode_(0) {}

void WinConsoleInput::Initialize() {
  // Get the standard input handle.
  std_input_handle_ = GetStdHandle(STD_INPUT_HANDLE);
  if (std_input_handle_ == INVALID_HANDLE_VALUE) {
    return;
  }
  // Save the current input mode.
  if (!GetConsoleMode(std_input_handle_, &previous_console_mode_)) {
    return;
  }
  // Set the console input mode to handle events.
  const DWORD console_mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
  SetConsoleMode(std_input_handle_, console_mode);
}

void WinConsoleInput::Update() {
  // Wait for new events.
  DWORD numRecords = 0;
  if (!ReadConsoleInput(std_input_handle_, input_buffer_, 128, &numRecords)) {
    return;
  }

  for (DWORD i = 0; i < numRecords; ++i) {
    switch (input_buffer_[i].EventType) {
      case KEY_EVENT: {
        const auto& key_event = input_buffer_[i].Event.KeyEvent;
        const Key& key = key_event.uChar.AsciiChar;
        if (key_states_.find(key) == key_states_.end()) {
          key_states_[key] = false;
        }
        if (key_event.bKeyDown && !key_states_[key]) {
          on_key_down_(key);
          key_states_[key] = true;
        } else if (!key_event.bKeyDown && key_states_[key]) {
          key_states_[key] = false;
          on_key_up_(key);
        }
      } break;
      default:
        // Unsupported input event.
        break;
    }
  }
}

void WinConsoleInput::Shutdown() {
  // Restore the console input mode.
  SetConsoleMode(std_input_handle_, previous_console_mode_);
}

void WinConsoleInput::SetOnKeyDownCallback(OnKeyDownCallback&& on_key_down) {
  on_key_down_ = std::move(on_key_down);
}

void WinConsoleInput::SetOnKeyUpCallback(OnKeyUpCallback&& on_key_up) {
  on_key_up_ = std::move(on_key_up);
}

}  // namespace examples
}  // namespace barelyapi
