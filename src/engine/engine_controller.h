#ifndef BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_

#include <cassert>
#include <cstdint>
#include <optional>

#include "core/time.h"
#include "engine/engine_state.h"
#include "engine/instrument_controller.h"
#include "engine/performer_controller.h"

namespace barely {

class EngineController {
 public:
  explicit EngineController(EngineState& engine) noexcept
      : engine_(engine), instrument_controller_(engine_), performer_controller_(engine_) {}

  void SetControl(BarelyEngineControlType type, float value) noexcept {
    engine_.ScheduleCmd(EngineControlCmd{type, kEngineControls[type].Clamp(value)});
  }

  void Update(double timestamp) noexcept {
    std::optional<int32_t> min_priority = std::nullopt;
    while (engine_.timestamp < timestamp) {
      if (engine_.tempo > 0.0) {
        const double max_update_duration =
            SecondsToBeats(engine_.tempo, timestamp - engine_.timestamp);

        double update_duration = max_update_duration;
        int32_t max_priority = INT32_MIN;
        performer_controller_.GetNextTaskEvent(min_priority, update_duration, max_priority);

        if (update_duration > 0.0) {
          performer_controller_.UpdatePosition(update_duration);
          engine_.timestamp += BeatsToSeconds(engine_.tempo, update_duration);
          min_priority = std::nullopt;
        }
        if (update_duration < max_update_duration) {
          performer_controller_.ProcessAllTasksAtPosition(min_priority, max_priority);
          min_priority = max_priority;
        }
      } else if (engine_.timestamp < timestamp) {
        engine_.timestamp = timestamp;
      }
    }
  }

  [[nodiscard]] InstrumentController& instrument_controller() noexcept {
    return instrument_controller_;
  }
  [[nodiscard]] const InstrumentController& instrument_controller() const noexcept {
    return instrument_controller_;
  }

  [[nodiscard]] PerformerController& performer_controller() noexcept {
    return performer_controller_;
  }
  [[nodiscard]] const PerformerController& performer_controller() const noexcept {
    return performer_controller_;
  }

 private:
  EngineState& engine_;
  InstrumentController instrument_controller_;
  PerformerController performer_controller_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
