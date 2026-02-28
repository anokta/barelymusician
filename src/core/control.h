#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

#include "core/constants.h"

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

inline float GetFrequency(float sample_rate, float cutoff) noexcept {
  static constexpr float kMinFreqInverse = 1.0f / kMinFilterFreq;
  const float max_freq = 0.5f * sample_rate;
  return std::min(kMinFilterFreq * std::pow(max_freq * kMinFreqInverse, cutoff), max_freq);
}

inline float GetFilterQ(float resonance) noexcept {
  static constexpr float kMinQ = 0.05f;
  static constexpr float kMinQInverse = 1.0f / kMinQ;
  static constexpr float kMaxQ = 10.0f;
  return std::min(kMinQ * std::pow(kMaxQ * kMinQInverse, resonance), kMaxQ);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONTROL_H_
