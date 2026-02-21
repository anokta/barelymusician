#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

#include "core/decibels.h"

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

inline float GetFrequency(float sample_rate, float normalized_cutoff) noexcept {
  static constexpr float kMinHz = 20.0f;
  static constexpr float kMinHzInverse = 1.0f / kMinHz;
  const float max_hz = 0.5f * sample_rate;
  return std::min(kMinHz * std::pow(max_hz * kMinHzInverse, normalized_cutoff), max_hz);
}

inline float GetGainDb(float normalized_gain) noexcept {
  static constexpr float kMaxDecibels = -6.0f;
  static constexpr float kDecibelsRange = kMaxDecibels - kMinDecibels;
  return kMinDecibels + kDecibelsRange * normalized_gain;
}

inline float GetGain(float normalized_gain) noexcept {
  return DecibelsToAmplitude(GetGainDb(normalized_gain));
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONTROL_H_
