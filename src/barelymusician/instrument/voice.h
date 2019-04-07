#ifndef BARELYMUSICIAN_INSTRUMENT_VOICE_H_
#define BARELYMUSICIAN_INSTRUMENT_VOICE_H_

#include "barelymusician/dsp/generator.h"

namespace barelyapi {

// Voice interface for a virtual instrument that generates output samples with
// respect to the note input.
class Voice : public Generator {
 public:
  // Returns whether the voice is currently active (i.e., playing).
  //
  // @return True if active.
  virtual bool IsActive() const = 0;

  // Starts the voice.
  virtual void Start() = 0;

  // Stops the voice.
  virtual void Stop() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_VOICE_H_
