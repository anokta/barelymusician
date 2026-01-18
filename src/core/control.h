#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <cassert>
#include <limits>

namespace barely {

struct Control {
  float value = 0.0f;
  float min_value = std::numeric_limits<float>::lowest();
  float max_value = std::numeric_limits<float>::max();

  constexpr Control() noexcept = default;

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

  constexpr bool SetValue(float new_value) noexcept {
    new_value = std::clamp(new_value, min_value, max_value);
    if (value != new_value) {
      value = new_value;
      return true;
    }
    return false;
  }
};

inline constexpr void ApproachValue(float& current_value, float target_value) noexcept {
  constexpr float kSmoothingCoeff = 0.002f;
  current_value += (target_value - current_value) * kSmoothingCoeff;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONTROL_H_
