#ifndef BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_

#include <cassert>
#include <cstdint>

#include "core/time.h"
#include "engine/engine_state.h"
#include "engine/instrument_controller.h"
#include "engine/performer_controller.h"

namespace barely {

class EngineController {
 public:
  explicit EngineController(EngineState& engine) noexcept
      : engine_(engine), instrument_controller_(engine_), performer_controller_(engine_) {}

  /// Sets a control value.
  void SetControl(BarelyEngineControlType type, float value) noexcept {
    if (auto& control = engine_.controls[type]; control.SetValue(value)) {
      engine_.ScheduleMessage(EngineControlMessage{type, control.value});
    }
  }

  /// Updates the engine at timestamp.
  ///
  /// @param instrument_controller Instrument controller.
  /// @param performer_controller Performer controller.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept {
    while (engine_.timestamp < timestamp) {
      if (engine_.tempo > 0.0) {
        const double max_update_duration =
            SecondsToBeats(engine_.tempo, timestamp - engine_.timestamp);

        double update_duration = max_update_duration;
        int32_t max_priority = INT32_MIN;

        performer_controller_.GetNextTaskEvent(update_duration, max_priority);
        if (const double next_duration = instrument_controller_.GetNextDuration();
            next_duration < update_duration) {
          update_duration = next_duration;
          max_priority = INT32_MAX;
        }

        if (update_duration > 0) {
          performer_controller_.Update(update_duration);
          instrument_controller_.Update(update_duration);

          engine_.timestamp += BeatsToSeconds(engine_.tempo, update_duration);
        }

        if (update_duration < max_update_duration) {
          performer_controller_.ProcessAllTasksAtPosition(max_priority);
          if (max_priority == INT32_MAX) {
            instrument_controller_.ProcessArp();
          }
        }
      } else if (engine_.timestamp < timestamp) {
        engine_.timestamp = timestamp;
      }
    }
  }

  InstrumentController& instrument_controller() noexcept { return instrument_controller_; }
  const InstrumentController& instrument_controller() const noexcept {
    return instrument_controller_;
  }

  PerformerController& performer_controller() noexcept { return performer_controller_; }
  const PerformerController& performer_controller() const noexcept { return performer_controller_; }

 private:
  EngineState& engine_;
  InstrumentController instrument_controller_;
  PerformerController performer_controller_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
