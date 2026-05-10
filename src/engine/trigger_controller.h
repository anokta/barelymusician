#ifndef BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_

#include <barelymusician.h>

#include <algorithm>
#include <cstdint>

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

  void Start(uint32_t trigger_index, double offset, double interval) noexcept {
    TriggerState& trigger = engine_.GetTrigger(trigger_index);
    trigger.offset = offset;
    trigger.interval = interval;
    trigger.is_playing = true;  // TODO(schedule): this should be false with positive offset
  }

  void Stop(uint32_t trigger_index) noexcept {
    engine_.GetTrigger(trigger_index).is_playing = false;
  }

  void ProcessAllTriggersAtPosition() noexcept {
    for (uint32_t i = 0; i < engine_.trigger_pool.ActiveCount(); ++i) {
      TriggerState& trigger = engine_.GetTrigger(engine_.trigger_pool.GetActive(i));
      if (trigger.is_playing && trigger.offset == 0.0) {
        trigger.callback();
        if (trigger.interval > 0.0) {
          trigger.offset = trigger.interval;
        } else {
          trigger.is_playing = false;
        }
      }
    }
  }

  void Update(double duration) noexcept {
    for (uint32_t i = 0; i < engine_.trigger_pool.ActiveCount(); ++i) {
      TriggerState& trigger = engine_.GetTrigger(engine_.trigger_pool.GetActive(i));
      if (trigger.is_playing) {
        assert(trigger.offset >= duration);
        trigger.offset -= duration;
      }
    }
  }

  [[nodiscard]] double GetNextDuration() const noexcept {
    double next_duration = std::numeric_limits<double>::max();
    for (uint32_t i = 0; i < engine_.trigger_pool.ActiveCount(); ++i) {
      const TriggerState& trigger = engine_.GetTrigger(engine_.trigger_pool.GetActive(i));
      if (trigger.is_playing) {
        next_duration = std::min(trigger.offset, next_duration);
      }
    }
    return next_duration;
  }

 private:
  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TRIGGER_CONTROLLER_H_
