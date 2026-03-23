#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <cassert>
#include <cmath>

#include "core/constants.h"

namespace barely {

struct Control {
  float value = 0.0f;
  float min_value = 0.0f;
  float max_value = 0.0f;

  constexpr Control() noexcept = default;

  template <typename ValueType>
  constexpr Control(ValueType default_value, ValueType min_value, ValueType max_value) noexcept
      : value(static_cast<float>(default_value)),
        min_value(static_cast<float>(min_value)),
        max_value(static_cast<float>(max_value)) {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
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

inline void ApproachValue(float& current_value, float target_value, float coeff) noexcept {
  current_value = target_value + coeff * (current_value - target_value);
}

[[nodiscard]] inline float GetCoefficient(float sample_rate, float seconds) noexcept {
  const float samples = sample_rate * seconds;
  static const float kLogEpsilon = std::log(kEnvelopeEpsilon);
  return (samples > 0.0f) ? std::exp(kLogEpsilon / samples) : 0.0f;
}

[[nodiscard]] inline float GetFrequency(float cutoff, float max_freq) noexcept {
  static constexpr float kMinFreqInverse = 1.0f / kMinFilterFreq;
  return std::min(kMinFilterFreq * std::pow(max_freq * kMinFreqInverse, cutoff), max_freq);
}

#define BARELY_DEFINE_CONTROL(EnumType, Name, Default, Min, Max, Label) Control(Default, Min, Max),

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONTROL_H_
