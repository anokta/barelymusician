#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Generic conductor interface.
class Conductor {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Conductor() = default;

  // Transforms note.
  //
  // @param note Original note.
  // @param energy Energy in range [0.0f, 1.0f].
  // @param stress Stress in range [0.0f, 1.0f].
  // @return Transformed raw note.
  virtual RawNote TransformNote(const Note& note, float energy,
                                float stress) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
