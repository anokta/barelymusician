#ifndef BARELYMUSICIAN_CORE_CONTROL_H_
#define BARELYMUSICIAN_CORE_CONTROL_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "core/constants.h"

namespace barely {

struct Control {
  double min_value = 0.0;
  double max_value = 0.0;

  constexpr Control() noexcept = default;
  template <typename ValueType>
  constexpr Control(ValueType min_value, ValueType max_value) noexcept
      : min_value(static_cast<double>(min_value)), max_value(static_cast<double>(max_value)) {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
                  "ValueType is not supported");
  }

  [[nodiscard]] constexpr double Clamp(double value) const noexcept {
    return static_cast<double>(std::clamp(value, min_value, max_value));
  }
};

inline void ApproachValue(double& current_value, double target_value, double coeff) noexcept {
  current_value = target_value + coeff * (current_value - target_value);
}

[[nodiscard]] inline double GetCoefficient(double sample_rate, double seconds) noexcept {
  const double samples = sample_rate * seconds;
  static const double kLogEpsilon = std::log(kEnvelopeEpsilon);
  return (samples > 0.0) ? std::exp(kLogEpsilon / samples) : 0.0;
}

[[nodiscard]] inline double GetFrequency(double cutoff, double max_freq) noexcept {
  static constexpr double kMinFreqInverse = 1.0 / kMinFilterFreq;
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
