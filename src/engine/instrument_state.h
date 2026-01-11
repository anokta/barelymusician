#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <limits>

#include "core/callback.h"
#include "dsp/control.h"

namespace barely {

// Returns a control array with overrides.
[[nodiscard]] inline InstrumentControlArray BuildControlArray(
    const BarelyInstrumentControlOverride* control_overrides,
    int32_t control_override_count) noexcept {
  InstrumentControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                   // kGain
      Control(0.0f),                               // kPitchShift
      Control(false),                              // kRetrigger
      Control(0.0f, -1.0f, 1.0f),                  // kStereoPan
      Control(8, 1, 16),                           // kVoiceCount
      Control(0.0f, 0.0f, 60.0f),                  // kAttack
      Control(0.0f, 0.0f, 60.0f),                  // kDecay
      Control(1.0f, 0.0f, 1.0f),                   // kSustain
      Control(0.0f, 0.0f, 60.0f),                  // kRelease
      Control(0.0f, 0.0f, 1.0f),                   // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),     // kOscMode
      Control(0.0f, 0.0f, 1.0f),                   // kOscNoiseMix
      Control(0.0f),                               // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                   // kOscShape
      Control(0.0f, -0.5f, 0.5f),                  // kOscSkew
      Control(0, 0, BarelySliceMode_kCount - 1),   // kSliceMode
      Control(16.0f, 1.0f, 16.0f),                 // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                   // kBitCrusherRate
      Control(0.0f, 0.0f, 1.0f),                   // kDistortionAmount
      Control(1.0f, 0.0f, 20.0f),                  // kDistortionDrive
      Control(0, 0, BarelyFilterType_kCount - 1),  // kFilterType
      Control(0.0f, 0.0f),                         // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),              // kFilterQ
      Control(0.0f, 0.0f, 1.0f),                   // kDelaySend
      Control(0.0f, -1.0f, 1.0f),                  // kSidechainSend
      Control(0, 0, BarelyArpMode_kCount - 1),     // kArpMode
      Control(0.5f, 0.001f, 1.0f),                 // kArpGateRatio
      Control(1.0f, 0.0f, 16.0f),                  // kArpRate
  };
  for (int i = 0; i < control_override_count; ++i) {
    control_array[control_overrides[i].type].SetValue(control_overrides[i].value);
  }
  return control_array;
}

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
