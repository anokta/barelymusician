#ifndef BARELYMUSICIAN_ENSEMBLE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENSEMBLE_CONDUCTOR_H_

#include "barelymusician/base/module.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

// Conductor interface that transforms given notes with respect to its set of
// configuration.
class Conductor : public Module {
 public:
  // Sets the energy (arousal) of score.
  //
  // @param energy Energy in range [-1, 1].
  virtual void SetEnergy(float energy) = 0;

  // Sets the root note (key) of score.
  //
  // @param Root note index.
  virtual void SetRootNote(float index) = 0;

  // Sets the stress (valence) of score.
  //
  // @param stress Stress in range [-1, 1].
  virtual void SetStress(float stress) = 0;

  // Conducts the given |note|.
  //
  // @param note Original note.
  // @return Transformed note.
  virtual Note TransformNote(const Note& note) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENSEMBLE_CONDUCTOR_H_
