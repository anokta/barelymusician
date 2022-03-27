#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Class that wraps note.
struct Note {
 public:
  /// Definition alias.
  using Definition = barely::NoteDefinition;

  /// Pitch definition alias.
  using PitchDefinition = barely::NotePitchDefinition;

  /// Pitch type.
  using PitchType = barely::NotePitchType;

  /// Duration.
  double duration;

  /// Pitch.
  double pitch;

  /// Intensity.
  double intensity;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_H_
