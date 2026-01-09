#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <limits>

#include "core/callback.h"
#include "dsp/control.h"

namespace barely {

struct InstrumentState {
  // Array of controls.
  InstrumentControlArray controls = {};
  std::vector<float> pitches = {};  // sorted

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls = {};

  // Note event callback.
  Callback<BarelyNoteEventCallback> note_event_callback = {};

  // Arpeggiator state.
  double arp_phase = 0.0;
  float arp_pitch = 0.0f;
  int arp_pitch_index = -1;
  bool is_arp_pitch_on = false;

  void Update(double duration) noexcept {
    if (IsArpEnabled() && !pitches.empty()) {
      assert(duration <= GetNextArpDuration());
      const double rate = static_cast<double>(controls[BarelyInstrumentControlType_kArpRate].value);
      arp_phase = std::fmod(arp_phase + duration * rate, 1.0);
    }
  }

  double GetNextArpDuration() const noexcept {
    if (!IsArpEnabled() || pitches.empty()) {
      return std::numeric_limits<double>::max();
    }
    const double rate = static_cast<double>(controls[BarelyInstrumentControlType_kArpRate].value);
    if (rate <= 0.0) {
      return std::numeric_limits<double>::max();
    }
    const double ratio =
        static_cast<double>(controls[BarelyInstrumentControlType_kArpGateRatio].value);
    if (!is_arp_pitch_on) {
      return (arp_phase < ratio) ? 0.0 : (1.0 - arp_phase) / rate;
    }
    return (ratio - arp_phase) / rate;
  }

  /// Returns whether the arpeggiator is enabled.
  bool IsArpEnabled() const noexcept {
    return static_cast<BarelyArpMode>(controls[BarelyInstrumentControlType_kArpMode].value) !=
           BarelyArpMode_kNone;
  }

  /// Returns whether a note is on or not.
  bool IsNoteOn(float pitch) const noexcept {
    return is_arp_pitch_on ? (arp_pitch == pitch) : note_controls.contains(pitch);
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
