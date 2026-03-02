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
      Control(1.0f, 0.0f, 1.0f),                  // kGain
      Control(0.0f),                              // kPitchShift
      Control(0.0f, -1.0f, 1.0f),                 // kStereoPan
      Control(0.0f, 0.0f, 8.0f),                  // kAttack
      Control(0.0f, 0.0f, 8.0f),                  // kDecay
      Control(1.0f, 0.0f, 1.0f),                  // kSustain
      Control(0.0f, 0.0f, 8.0f),                  // kRelease
      Control(0, 0, BarelySliceMode_kCount - 1),  // kSliceMode
      Control(0.0f, 0.0f, 1.0f),                  // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),    // kOscMode
      Control(0.0f, 0.0f, 1.0f),                  // kOscNoiseMix
      Control(0.0f),                              // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                  // kOscShape
      Control(0.0f, -1.0f, 1.0f),                 // kOscSkew
      Control(0.0f, 0.0f, 1.0f),                  // kCrushDepth
      Control(0.0f, 0.0f, 1.0f),                  // kCrushRate
      Control(0.0f, 0.0f, 1.0f),                  // kDistortionMix
      Control(0.0f, 0.0f, 1.0f),                  // kDistortionDrive
      Control(1.0f, 0.0f, 1.0f),                  // kFilterCutoff
      Control(0.5f, 0.0f, 1.0f),                  // kFilterResonance
      Control(0.0f, -1.0f, 1.0f),                 // kFilterTone
      Control(0.0f, 0.0f, 1.0f),                  // kDelaySend
      Control(0.0f, 0.0f, 2.0f),                  // kReverbSend
      Control(0.0f, -1.0f, 1.0f),                 // kSidechainSend
      Control(0, 0, BarelyArpMode_kCount - 1),    // kArpMode
      Control(0.5f, 0.0f, 1.0f),                  // kArpGate
      Control(1.0f, 0.0f, 16.0f),                 // kArpRate
      Control(false),                             // kRetrigger
      Control(8, 1, 16),                          // kVoiceCount
  };

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
