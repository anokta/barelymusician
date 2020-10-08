#include "input_manager/win_console_input.h"

#include <utility>

namespace barelyapi {
namespace examples {

WinConsoleInput::WinConsoleInput()
    : std_input_handle_(nullptr),
      previous_console_mode_(0),
      key_down_callback_(nullptr),
      key_up_callback_(nullptr) {}

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

void WinConsoleInput::Shutdown() {
  // Restore the console input mode.
  SetConsoleMode(std_input_handle_, previous_console_mode_);
}

void WinConsoleInput::Update() {
  DWORD num_events = 0;
  if (!GetNumberOfConsoleInputEvents(std_input_handle_, &num_events) ||
      num_events == 0) {
    return;
  }
  if (!ReadConsoleInput(std_input_handle_, input_buffer_, 128, &num_events)) {
    return;
  }

  for (DWORD i = 0; i < num_events; ++i) {
    switch (input_buffer_[i].EventType) {
      case KEY_EVENT: {
        const auto& key_event = input_buffer_[i].Event.KeyEvent;
        const Key& key = key_event.uChar.AsciiChar;
        if (key_states_.find(key) == key_states_.cend()) {
          key_states_[key] = false;
        }
        if (key_event.bKeyDown && !key_states_[key]) {
          key_states_[key] = true;
          if (key_down_callback_ != nullptr) {
            key_down_callback_(key);
          }
        } else if (!key_event.bKeyDown && key_states_[key]) {
          key_states_[key] = false;
          if (key_up_callback_ != nullptr) {
            key_up_callback_(key);
          }
        }
      } break;
      default:
        // Unsupported input event.
        break;
    }
  }
}

void WinConsoleInput::SetKeyDownCallback(KeyDownCallback&& key_down_callback) {
  key_down_callback_ = std::move(key_down_callback);
}

void WinConsoleInput::SetKeyUpCallback(KeyUpCallback&& key_up_callback) {
  key_up_callback_ = std::move(key_up_callback);
}

}  // namespace examples
}  // namespace barelyapi
