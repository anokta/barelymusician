
#include "barelymusician/engine/conductor.h"

#include <cmath>
#include <utility>

#include "barelymusician/engine/note.h"

namespace barelyapi {

namespace {

// Returns note pitch for a given `scale` and `index`.
double GetPitch(const std::vector<double>& scale, int index) noexcept {
  if (!scale.empty()) {
    const double scale_length = static_cast<double>(scale.size());
    const double octave_offset =
        std::floor(static_cast<double>(index) / scale_length);
    const int scale_offset =
        index - static_cast<int>(octave_offset * scale_length);
    return octave_offset + scale[scale_offset];
  }
  return 0.0;
}

}  // namespace

double Conductor::GetNote(Note::PitchDefinition definition) const noexcept {
  switch (static_cast<Note::PitchType>(definition.type)) {
    case Note::PitchType::kAbsolutePitch:
      return definition.absolute_pitch;
    case Note::PitchType::kRelativePitch:
      return root_pitch_ + definition.relative_pitch;
    case Note::PitchType::kScaleIndex:
      return root_pitch_ + GetPitch(scale_pitches_, definition.scale_index);
  }
  return 0.0;
}

double Conductor::GetRootNote() const noexcept { return root_pitch_; }

const std::vector<double>& Conductor::GetScale() const noexcept {
  return scale_pitches_;
}

void Conductor::SetAdjustNoteCallback(AdjustNoteCallback callback) noexcept {
  adjust_note_callback_ = std::move(callback);
}

void Conductor::SetRootNote(double root_pitch) noexcept {
  root_pitch_ = root_pitch;
}

void Conductor::SetScale(std::vector<double> scale_pitches) noexcept {
  scale_pitches_ = std::move(scale_pitches);
}

Note Conductor::TransformNote(Note::Definition definition,
                              bool skip_adjustment) const noexcept {
  if (adjust_note_callback_ && !skip_adjustment) {
    adjust_note_callback_(&definition);
  }
  return Note{definition.duration, GetNote(definition.pitch),
              definition.intensity};
}

}  // namespace barelyapi
