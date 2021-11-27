#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_H_

#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"

namespace barely {

/// Musical note.
struct Note {
  /// Note pitch.
  NotePitch pitch;

  /// Note intensity.
  NoteIntensity intensity;

  /// Note duration.
  NoteDuration duration;

  /// Default comparators.
  auto operator<=>(const Note& other) const noexcept = default;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_H_
