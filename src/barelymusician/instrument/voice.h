#ifndef BARELYMUSICIAN_INSTRUMENT_VOICE_H_
#define BARELYMUSICIAN_INSTRUMENT_VOICE_H_

#include "barelymusician/base/unit_generator.h"

namespace barelyapi {

// Voice interface for a virtual instrument that generates output samples with
// respect to the note input.
class Voice : public UnitGenerator {
 public:
  // Returns whether the voice is currently active (i.e., playing).
  //
  // @return True if active.
  virtual bool IsActive() const = 0;

  // Stops the voice for the given note input.
  //
  // @param index Note index to be stopped.
  virtual void NoteOff(float index) = 0;

  // Starts the voice for the given note input.
  //
  // @param index Note index to be played.
  // @param gain Note gain in amplitude.
  virtual void NoteOn(float index, float gain) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_VOICE_H_
