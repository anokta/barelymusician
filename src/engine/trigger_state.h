#ifndef BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_
#define BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_

#include <barelymusician.h>

#include "core/callback.h"

namespace barely {

struct TriggerState {
  Callback<BarelyTriggerCallback> callback = {};

  double offset = 0.0;
  double interval = 0.0;

  bool is_playing = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TRIGGER_STATE_H_
