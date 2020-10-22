#ifndef BARELYMUSICIAN_ENGINE_CLIP_H_
#define BARELYMUSICIAN_ENGINE_CLIP_H_

#include <vector>

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Musical clip.
struct Clip {
  // Start position in beats.
  Position position;

  // Length in beats.
  Position length;

  // Is looping?
  bool loop;

  // Notes.
  std::vector<Note> notes;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLIP_H_
