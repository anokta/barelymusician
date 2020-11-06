#include "examples/util/input_manager.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace examples {

namespace {

#if defined(__APPLE__)
// Keyboard key press event mask.
constexpr CGEventMask kEventMask =
    (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);
#endif  // defined(__APPLE__)

}  // namespace

InputManager::InputManager()
    : key_down_callback_(nullptr), key_up_callback_(nullptr) {
#if defined(__APPLE__)
  event_callback_ = [this](CGEventType type, CGEventRef event) {
    UniCharCount length = 0;
    UniChar str[1];
    CGEventKeyboardGetUnicodeString(event, 1, &length, str);
    DCHECK_EQ(length, 1);
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
  event_tap_ = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap,
                                kCGEventTapOptionDefault, kEventMask, callback,
                                reinterpret_cast<void*>(&event_callback_));
  DCHECK(event_tap_);
  run_loop_source_ =
      CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap_, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), run_loop_source_,
                     kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap_, true);
#endif  // defined(__APPLE__)
}

InputManager::~InputManager() {
#if defined(__APPLE__)
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
#if defined(__APPLE__)
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

}  // namespace examples
}  // namespace barelyapi
