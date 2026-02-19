#ifndef BARELYMUSICIAN_ENGINE_NOTE_STATE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_STATE_H_

#include <barelymusician.h>

#include <array>

#include "core/constants.h"
#include "core/control.h"

namespace barely {

using NoteControlArray = std::array<Control, BarelyNoteControlType_kCount>;

[[nodiscard]] inline NoteControlArray BuildNoteControlArray(
    const BarelyNoteControlOverride* note_control_overrides,
    int32_t note_control_override_count) noexcept {
  NoteControlArray note_control_array = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };
  for (int i = 0; i < note_control_override_count; ++i) {
    note_control_array[note_control_overrides[i].type].SetValue(note_control_overrides[i].value);
  }
  return note_control_array;
}

struct NoteState {
  NoteControlArray controls = BuildNoteControlArray(nullptr, 0);
  float pitch = 0.0;

  uint32_t prev_note_index = kInvalidIndex;
  uint32_t next_note_index = kInvalidIndex;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_NOTE_STATE_H_
