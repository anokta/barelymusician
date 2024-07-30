
#include "barelymusician/internal/note.h"

#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"

namespace barely::internal {

Note::Note(int note_id, double pitch, double intensity, ControlMap control_map) noexcept
    : note_id_(note_id),
      pitch_(pitch),
      intensity_(intensity),
      control_map_(std::move(control_map)) {}

Control* Note::GetControl(int control_id) noexcept { return FindOrNull(control_map_, control_id); }

int Note::GetId() const noexcept { return note_id_; }

double Note::GetIntensity() const noexcept { return intensity_; }

double Note::GetPitch() const noexcept { return pitch_; }

}  // namespace barely::internal
