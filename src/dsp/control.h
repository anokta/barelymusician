#ifndef BARELYMUSICIAN_DSP_CONTROL_H_
#define BARELYMUSICIAN_DSP_CONTROL_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>

namespace barely {

/// Control.
struct Control {
  /// Constructs a new `Control`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  template <typename ValueType>
  constexpr Control(ValueType default_value,
                    ValueType min_value = std::numeric_limits<ValueType>::lowest(),
                    ValueType max_value = std::numeric_limits<ValueType>::max()) noexcept
      : value(static_cast<float>(default_value)),
        min_value(static_cast<float>(min_value)),
        max_value(static_cast<float>(max_value)) {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    assert(default_value >= min_value && default_value <= max_value);
  }

  /// Sets the value.
  ///
  /// @param new_value New value.
  /// @return True if the value has changed, false otherwise.
  constexpr bool SetValue(float new_value) noexcept {
    new_value = std::clamp(new_value, min_value, max_value);
    if (value != new_value) {
      value = new_value;
      return true;
    }
    return false;
  }

  /// Value.
  float value = 0.0f;

  /// Minimum value.
  float min_value = std::numeric_limits<float>::lowest();

  /// Maximum value.
  float max_value = std::numeric_limits<float>::max();
};

/// Control array.
using ControlArray = std::array<Control, BarelyControlType_kCount>;

/// Engine control array.
using EngineControlArray = std::array<Control, BarelyEngineControlType_kCount>;

/// Note control array.
using NoteControlArray = std::array<Control, BarelyNoteControlType_kCount>;

/// Approaches a value to a target value with smoothing.
///
/// @param current_value Current value.
/// @param target_value Target value.
inline constexpr void ApproachValue(float& current_value, float target_value) noexcept {
  constexpr float kSmoothingCoeff = 0.002f;
  current_value += (target_value - current_value) * kSmoothingCoeff;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_CONTROL_H_
