#include "examples/common/input_manager.h"

#include <utility>

namespace barely::examples {

InputManager::InputManager()
    : key_down_callback_(nullptr), key_up_callback_(nullptr) {
#if defined(_WIN32) || defined(__CYGWIN__)
  std_input_handle_ = GetStdHandle(STD_INPUT_HANDLE);
  if (std_input_handle_ == INVALID_HANDLE_VALUE) {
    return;
  }
  if (!GetConsoleMode(std_input_handle_, &previous_console_mode_)) {
    return;
  }
  const DWORD console_mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
  SetConsoleMode(std_input_handle_, console_mode);
#elif defined(__APPLE__)
  event_callback_ = [this](CGEventType type, CGEventRef event) {
    UniCharCount length = 0;
    UniChar str[1];
    CGEventKeyboardGetUnicodeString(event, 1, &length, str);
    if (length != 1) {
      return;
    }
    const Key key = static_cast<Key>(str[0]);
    if (type == kCGEventKeyDown) {
      HandleKeyDown(key);
    } else if (type == kCGEventKeyUp) {
      HandleKeyUp(key);
    }
  };
  const auto callback = [](CGEventTapProxy proxy, CGEventType type,
                           CGEventRef event, void* refcon) -> CGEventRef {
    if (refcon) {
      // Access the event callback via |refcon| (to avoid capturing
      // |event_callback_|).
      const auto& event_callback = *reinterpret_cast<EventCallback*>(refcon);
      event_callback(type, event);
    }
    return event;
  };
  const CGEventMask event_mask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);
  event_tap_ = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap,
                                kCGEventTapOptionDefault, event_mask, callback,
                                reinterpret_cast<void*>(&event_callback_));
  if (!event_tap_) {
    return;
  }
  run_loop_source_ =
      CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap_, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), run_loop_source_,
                     kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap_, true);
#endif  // defined(__APPLE__)
}

InputManager::~InputManager() {
#if defined(_WIN32) || defined(__CYGWIN__)
  SetConsoleMode(std_input_handle_, previous_console_mode_);
#elif defined(__APPLE__)
  CGEventTapEnable(event_tap_, false);
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), run_loop_source_,
                        kCFRunLoopCommonModes);
#endif  // defined(__APPLE__)
}

void InputManager::SetKeyDownCallback(KeyDownCallback key_down_callback) {
  key_down_callback_ = std::move(key_down_callback);
}

void InputManager::SetKeyUpCallback(KeyUpCallback key_up_callback) {
  key_up_callback_ = std::move(key_up_callback);
}

void InputManager::Update() {
#if defined(_WIN32) || defined(__CYGWIN__)
  DWORD num_events = 0;
  if (!GetNumberOfConsoleInputEvents(std_input_handle_, &num_events) ||
      num_events == 0) {
    return;
  }
  if (!ReadConsoleInput(std_input_handle_, input_buffer_, 128, &num_events)) {
    return;
  }
  for (DWORD i = 0; i < num_events; ++i) {
    if (input_buffer_[i].EventType == KEY_EVENT) {
      const auto& key_event = input_buffer_[i].Event.KeyEvent;
      const Key& key = key_event.uChar.AsciiChar;
      if (key_event.bKeyDown) {
        HandleKeyDown(key);
      } else {
        HandleKeyUp(key);
      }
    }
  }
#elif defined(__APPLE__)
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
#endif  // defined(__APPLE__)
}

void InputManager::HandleKeyDown(const Key& key) {
  if (pressed_keys_.insert(key).second && key_down_callback_) {
    key_down_callback_(key);
  }
}

void InputManager::HandleKeyUp(const Key& key) {
  if (pressed_keys_.erase(key) > 0 && key_up_callback_) {
    key_up_callback_(key);
  }
}

}  // namespace barely::examples
