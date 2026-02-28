#ifndef BARELYMUSICIAN_ENGINE_NOTE_STATE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_STATE_H_

#include <barelymusician.h>

#include <array>

#include "core/constants.h"
#include "core/control.h"

namespace barely {

struct NoteState {
  std::array<Control, BarelyNoteControlType_kCount> controls = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };

  float pitch = 0.0;

  uint32_t prev_note_index = kInvalidIndex;
  uint32_t next_note_index = kInvalidIndex;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_NOTE_STATE_H_
