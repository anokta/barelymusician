#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include "barelymusician/base/unit_generator.h"

namespace barelyapi {

// Generic instrument interface.
// TODO(#9): Should instruments process multiple frames in one go?
class Instrument : public UnitGenerator {
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

  // TODO(#13): Should return failure, i.e., return bool and pass param ref.
  // Gets the value of a float parameter with the given ID.
  //
  // @param id Parameter ID.
  // @return Float parameter value.
  virtual float GetFloatParam(int id) const = 0;

  // TODO(#13): Should return failure, i.e., return bool if id not found.
  // Sets the value of a float parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Float parameter value.
  virtual void SetFloatParam(int id, float value) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
