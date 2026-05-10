#ifndef BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_

#include <barelymusician.h>

#include <algorithm>
#include <cstdint>
#include <limits>

#include "engine/engine_state.h"
#include "engine/trigger_state.h"

namespace barely {

class TriggerController {
 public:
  explicit TriggerController(EngineState& engine) : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept {
    const uint32_t trigger_index = engine_.trigger_pool.Acquire();
    if (trigger_index != kInvalidIndex) {
      engine_.GetTrigger(trigger_index) = {};
    }
    return trigger_index;
  }

  void Release(uint32_t trigger_index) noexcept { engine_.trigger_pool.Release(trigger_index); }

  void SetCallback(uint32_t trigger_index, BarelyTriggerCallback callback,
                   void* user_data) noexcept {
    engine_.GetTrigger(trigger_index).callback = {callback, user_data};
  }

  void Start(uint32_t trigger_index, double next_position, double interval) noexcept {
    TriggerState& trigger = engine_.GetTrigger(trigger_index);
    trigger.next_position = next_position;
    trigger.interval = interval;
  }

  void Stop(uint32_t trigger_index) noexcept {
    engine_.GetTrigger(trigger_index).next_position = 0.0;
  }

  void ProcessAllTriggersAtPosition(double position) noexcept {
    for (uint32_t i = 0; i < engine_.trigger_pool.ActiveCount(); ++i) {
      if (TriggerState& trigger = engine_.GetTrigger(engine_.trigger_pool.GetActive(i));
          trigger.IsPlaying() && trigger.next_position <= position) {
        trigger.next_position =
            (trigger.interval > 0.0) ? (trigger.next_position + trigger.interval) : 0.0;
        trigger.callback();
      }
    }
  }

  [[nodiscard]] double GetNextDuration(double position) const noexcept {
    double next_position = std::numeric_limits<double>::max();
    for (uint32_t i = 0; i < engine_.trigger_pool.ActiveCount(); ++i) {
      if (const TriggerState& trigger = engine_.GetTrigger(engine_.trigger_pool.GetActive(i));
          trigger.IsPlaying()) {
        next_position = std::min(trigger.next_position, next_position);
      }
    }
    assert(next_position >= position);
    return next_position - position;
  }

 private:
  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_
