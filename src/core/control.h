#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "core/constants.h"

namespace barely {

struct Control {
  float min_value = 0.0f;
  float max_value = 0.0f;

  constexpr Control() noexcept = default;
  template <typename ValueType>
  constexpr Control(ValueType min_value, ValueType max_value) noexcept
      : min_value(static_cast<float>(min_value)), max_value(static_cast<float>(max_value)) {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
                  "ValueType is not supported");
  }

  [[nodiscard]] constexpr float Clamp(float value) const noexcept {
    return std::clamp(value, min_value, max_value);
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

#define BARELY_DEFINE_CONTROL(EnumType, Name, Default, Min, Max, Label) Control(Min, Max),
inline constexpr std::array<Control, BarelyEngineControlType_kCount> kEngineControls = {
    BARELY_ENGINE_CONTROL_TYPES(EngineControlType, BARELY_DEFINE_CONTROL)};
inline constexpr std::array<Control, BarelyInstrumentControlType_kCount> kInstrumentControls = {
    BARELY_INSTRUMENT_CONTROL_TYPES(InstrumentControlType, BARELY_DEFINE_CONTROL)};
inline constexpr std::array<Control, BarelyNoteControlType_kCount> kNoteControls = {
    BARELY_NOTE_CONTROL_TYPES(NoteControlType, BARELY_DEFINE_CONTROL)};
#undef BARELY_DEFINE_CONTROL

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONTROL_H_
