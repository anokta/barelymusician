#ifndef BARELYMUSICIAN_INTERNAL_CONTROL_H_
#define BARELYMUSICIAN_INTERNAL_CONTROL_H_

#include <cassert>
#include <functional>
#include <span>
#include <unordered_map>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely {

/// Control definition.
struct ControlDefinition {
  /// Constructs a new `ControlDefinition`.
  ///
  /// @param id Identifier.
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  template <typename IdType, typename ValueType>
  constexpr ControlDefinition(IdType id, ValueType default_value,
                              ValueType min_value = std::numeric_limits<ValueType>::lowest(),
                              ValueType max_value = std::numeric_limits<ValueType>::max()) noexcept
      : id(static_cast<int32_t>(id)),
        default_value(static_cast<double>(default_value)),
        min_value(static_cast<double>(min_value)),
        max_value(static_cast<double>(max_value)) {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    assert(default_value >= min_value && default_value <= max_value);
  }

  /// Identifier.
  int32_t id = 0;

  /// Default value.
  double default_value = 0.0;

  /// Minimum value.
  double min_value = 0.0;

  /// Maximum value.
  double max_value = 0.0;
};

/// Class that wraps a control.
class Control {
 public:
  /// Event alias.
  using Event = ::barely::Event<ControlEventDefinition, int, double>;

  /// Set value callback alias.
  using SetValueCallback = std::function<void(int, double)>;

  /// Constructs a new `Control`.
  ///
  /// @param definition Control definition.
  /// @param set_value_callback Set value callback.
  Control(ControlDefinition definition, SetValueCallback set_value_callback) noexcept;

  /// Returns the value.
  ///
  /// @return Control value.
  [[nodiscard]] double GetValue() const noexcept;

  /// Resets the value.
  void ResetValue() noexcept;

  /// Sets the value.
  ///
  /// @param value Control value.
  void SetValue(double value) noexcept;

 private:
  // Definition.
  ControlDefinition definition_;

  // Set value callback.
  SetValueCallback set_value_callback_;

  // Value.
  double value_ = 0.0;
};

/// Control map alias.
using ControlMap = std::unordered_map<int, Control>;

/// Builds the corresponding control map for a given array of control `definitions`.
///
/// @param definitions Spand of control definitions.
/// @param set_value_callback Set value callback.
/// @return Control map.
// NOLINTNEXTLINE(bugprone-exception-escape)
ControlMap BuildControlMap(std::span<const ControlDefinition> definitions,
                           const Control::SetValueCallback& set_value_callback) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_CONTROL_H_
