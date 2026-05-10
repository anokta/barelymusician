#ifndef BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_

#include <cassert>
#include <cstdint>

#include "core/time.h"
#include "engine/engine_state.h"
#include "engine/instrument_controller.h"
#include "engine/trigger_controller.h"

namespace barely {

class EngineController {
 public:
  explicit EngineController(EngineState& engine) noexcept
      : engine_(engine), instrument_controller_(engine_), trigger_controller_(engine_) {}

  void SetControl(BarelyEngineControlType type, float value) noexcept {
    if (auto& control = engine_.controls[type]; control.SetValue(value)) {
      engine_.ScheduleMessage(EngineControlMessage{type, control.value});
    }
  }

  void Update(double timestamp) noexcept {
    while (engine_.timestamp < timestamp) {
      if (engine_.tempo > 0.0) {
        const double max_update_duration =
            SecondsToBeats(engine_.tempo, timestamp - engine_.timestamp);
        const double update_duration =
            std::min(std::min(trigger_controller_.GetNextDuration(engine_.position),
                              instrument_controller_.GetNextDuration()),
                     max_update_duration);

        if (update_duration > 0) {
          instrument_controller_.Update(update_duration);
          engine_.timestamp += BeatsToSeconds(engine_.tempo, update_duration);
          engine_.position += update_duration;
        }

        if (update_duration < max_update_duration) {
          trigger_controller_.ProcessAllTriggersAtPosition(engine_.position);
          instrument_controller_.ProcessArp();
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

  [[nodiscard]] TriggerController& trigger_controller() noexcept { return trigger_controller_; }
  [[nodiscard]] const TriggerController& trigger_controller() const noexcept {
    return trigger_controller_;
  }

 private:
  EngineState& engine_;
  InstrumentController instrument_controller_;
  TriggerController trigger_controller_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
