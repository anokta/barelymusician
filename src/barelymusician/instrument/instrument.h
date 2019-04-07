#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include "barelymusician/base/types.h"
#include "barelymusician/dsp/generator.h"

namespace barelyapi {

// Generic instrument interface.
// TODO(#9): Should instruments process multiple frames in one go?
class Instrument : public Generator {
 public:
  // Plays note with the given index and intensity.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  virtual void NoteOn(float index, float intensity) = 0;

  // Stops note with the given index.
  //
  // @param index Note index.
  virtual void NoteOff(float index) = 0;

  // Sets the value of a float parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Float parameter value.
  virtual void SetFloatParam(ParamId id, float value) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
