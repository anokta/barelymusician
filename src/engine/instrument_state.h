#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_

#include <barelymusician.h>

#include "core/callback.h"
#include "dsp/control.h"
#include "engine/arp_state.h"

namespace barely {

struct InstrumentState {
  // Array of controls.
  InstrumentControlArray controls = {};
  std::vector<float> pitches = {};  // sorted

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls = {};

  // Arpeggiator state.
  ArpState arp = {};

  // Note event callback.
  Callback<BarelyNoteEventCallback> note_event_callback = {};

  void Update(double duration) noexcept {
    if (IsArpEnabled() && !pitches.empty()) {
      arp.Update(duration,
                 static_cast<double>(controls[BarelyInstrumentControlType_kArpRate].value),
                 static_cast<double>(controls[BarelyInstrumentControlType_kArpGateRatio].value));
    }
  }

  /// Returns whether the arpeggiator is enabled.
  bool IsArpEnabled() const noexcept {
    return static_cast<BarelyArpMode>(controls[BarelyInstrumentControlType_kArpMode].value) !=
           BarelyArpMode_kNone;
  }

  /// Returns whether a note is on or not.
  bool IsNoteOn(float pitch) const noexcept {
    return arp.is_active ? (arp.pitch == pitch) : note_controls.contains(pitch);
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
