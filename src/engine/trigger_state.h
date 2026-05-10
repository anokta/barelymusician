#ifndef BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_
#define BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_

#include <barelymusician.h>

#include <limits>

#include "core/callback.h"

namespace barely {

struct TriggerState {
  Callback<BarelyTriggerCallback> callback = {};

  double next_position = 0.0;
  double interval = 0.0;

  [[nodiscard]] bool IsPlaying() const noexcept { return next_position > 0.0; }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_
