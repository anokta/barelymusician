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
#include "engine/note_state.h"

namespace barely {

struct InstrumentState {
  std::array<Control, BarelyInstrumentControlType_kCount> controls = {
      BARELY_INSTRUMENT_CONTROL_TYPES(InstrumentControlType, BARELY_DEFINE_CONTROL)};

  Callback<BarelyNoteEventCallback> note_event_callback = {};

  struct {
    double phase = 0.0;
    uint32_t note_index = kInvalidIndex;
    bool is_note_on = false;
    bool should_release_note = false;
    bool should_update = false;
  } arp = {};

  uint32_t first_note_index = kInvalidIndex;
  uint32_t note_count = 0;

  uint32_t first_slice_index = kInvalidIndex;

  double GetNextArpDuration() const noexcept {
    if (first_note_index == kInvalidIndex || !IsArpEnabled()) {
      return std::numeric_limits<double>::max();
    }
    const double gate = static_cast<double>(controls[BarelyInstrumentControlType_kArpGate].value);
    const double rate = static_cast<double>(controls[BarelyInstrumentControlType_kArpRate].value);
    if (rate <= 0.0) {
      return std::numeric_limits<double>::max();
    }
    if (arp.is_note_on && gate > arp.phase) {
      return (gate - arp.phase) / rate;
    }
    return (arp.phase > 0.0 || arp.should_update) ? ((1.0 - arp.phase) / rate) : 0.0;
  }

  bool IsArpEnabled() const noexcept {
    return static_cast<BarelyArpMode>(controls[BarelyInstrumentControlType_kArpMode].value) !=
           BarelyArpMode_kNone;
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
