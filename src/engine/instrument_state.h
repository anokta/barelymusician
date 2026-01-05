#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <span>

#include "common/callback.h"
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

  /// Returns whether the arpeggiator is enabled.
  bool IsArpEnabled() const noexcept {
    return static_cast<BarelyArpMode>(controls[BarelyInstrumentControlType_kArpMode].value) !=
           BarelyArpMode_kNone;
  }

  /// Returns whether a note is on or not.
  bool IsNoteOn(float pitch) const noexcept {
    return arp.is_active ? (arp.pitch == pitch) : note_controls.contains(pitch);
  }

  void Update(double duration) noexcept {
    if (IsArpEnabled() && !pitches.empty()) {
      arp.Update(duration,
                 static_cast<double>(controls[BarelyInstrumentControlType_kArpRate].value),
                 static_cast<double>(controls[BarelyInstrumentControlType_kArpGateRatio].value));
    }
  }
};

// Returns a control array with overrides.
[[nodiscard]] inline InstrumentControlArray BuildControlArray(
    std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept {
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
  for (const auto& [type, value] : control_overrides) {
    control_array[type].SetValue(value);
  }
  return control_array;
}

// Returns a note control array with overrides.
[[nodiscard]] inline NoteControlArray BuildNoteControlArray(
    std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  NoteControlArray note_control_array = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };
  for (const auto& [type, value] : note_control_overrides) {
    note_control_array[type].SetValue(value);
  }
  return note_control_array;
}

// Returns an array of note control values from a given note control array.
[[nodiscard]] inline std::array<float, BarelyNoteControlType_kCount> BuildNoteControls(
    const NoteControlArray& note_control_array) noexcept {
  std::array<float, BarelyNoteControlType_kCount> note_controls;
  for (int i = 0; i < BarelyNoteControlType_kCount; ++i) {
    note_controls[i] = note_control_array[i].value;
  }
  return note_controls;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_STATE_H_
