#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Generic conductor interface.
class Conductor {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Conductor() = default;

  // Sets energy.
  //
  // @param energy Energy in range [0.0f, 1.0f].
  virtual void SetEnergy(float energy) = 0;

  // Sets stress.
  //
  // @param energy Stress in range [0.0f, 1.0f].
  virtual void SetStress(float stress) = 0;

  // Transforms note.
  //
  // @param note Original note.
  // @return Transformed note.
  virtual Note TransformNote(const Note& note) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
