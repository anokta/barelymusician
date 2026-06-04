#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cmath>
#include <limits>

#include "core/callback.h"
#include "core/constants.h"
#include "core/control.h"

namespace barely {

struct InstrumentState {
  std::array<Control, BarelyInstrumentControlType_kCount> controls = {
      BARELY_INSTRUMENT_CONTROL_TYPES(InstrumentControlType, BARELY_DEFINE_CONTROL)};

  Callback<BarelyNoteEventCallback> note_event_callback = {};

  uint32_t first_note_index = kInvalidIndex;
  uint32_t first_slice_index = kInvalidIndex;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
