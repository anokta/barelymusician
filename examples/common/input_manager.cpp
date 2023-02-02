#include "examples/common/input_manager.h"

#include <utility>

namespace barely::examples {

InputManager::InputManager() noexcept
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
  event_callback_ = [this](CGEventType type, CGEventRef event) noexcept {
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
  const auto callback = [](CGEventTapProxy /*proxy*/, CGEventType type,
                           CGEventRef event,
                           void* refcon) noexcept -> CGEventRef {
    if (refcon) {
      // Access the event callback via `refcon` (to avoid capturing
      // `event_callback_`).
      const auto& event_callback = *static_cast<EventCallback*>(refcon);
      event_callback(type, event);
    }
    return event;
  };
  const CGEventMask event_mask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);
  event_tap_ = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap,
                                kCGEventTapOptionDefault, event_mask, callback,
                                static_cast<void*>(&event_callback_));
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

InputManager::~InputManager() noexcept {
#if defined(_WIN32) || defined(__CYGWIN__)
  SetConsoleMode(std_input_handle_, previous_console_mode_);
#elif defined(__APPLE__)
  CGEventTapEnable(event_tap_, false);
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), run_loop_source_,
                        kCFRunLoopCommonModes);
#endif  // defined(__APPLE__)
}

void InputManager::SetKeyDownCallback(
    KeyDownCallback key_down_callback) noexcept {
  key_down_callback_ = std::move(key_down_callback);
}

void InputManager::SetKeyUpCallback(KeyUpCallback key_up_callback) noexcept {
  key_up_callback_ = std::move(key_up_callback);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InputManager::Update() noexcept {
#if defined(_WIN32) || defined(__CYGWIN__)
  for (int i = 0; i < 128; ++i) {
    const Key key = static_cast<Key>(i);
    if (GetKeyState(i) >> 1) {
      HandleKeyDown(key);
    } else {
      HandleKeyUp(key);
    }
  }
  FlushConsoleInputBuffer(std_input_handle_);
#elif defined(__APPLE__)
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
#endif  // defined(__APPLE__)
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InputManager::HandleKeyDown(const Key& key) noexcept {
  if (pressed_keys_.insert(key).second && key_down_callback_) {
    key_down_callback_(key);
  }
}

void InputManager::HandleKeyUp(const Key& key) noexcept {
  if (pressed_keys_.erase(key) > 0 && key_up_callback_) {
    key_up_callback_(key);
  }
}

}  // namespace barely::examples
