#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_queue.h"

namespace barelyapi {

// Class that performs a given instrument by sample accurate input directives.
class Performer {
 public:
  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(Instrument* instrument);

  // Plays note at the given |timestamp|.
  //
  // @param timestamp Timestamp to play the note.
  // @param index Note index.
  // @param intensity Note intensity.
  void PlayNote(int timestamp, float index, float intensity);

  // Stops note at the given |timestamp|.
  //
  // @param timestamp Timestamp to stop the note.
  // @param index Note index.
  void StopNote(int timestamp, float index);

  // Updates float parameter at the given |timestamp|.
  //
  // @param timestamp Timestamp to update the parameter.
  // @param id Parameter ID.
  // @param value Parameter value.
  void UpdateFloatParam(int timestamp, ParamId id, float value);

  // TODO(#20): Create and pass an AudioBuffer here instead?
  // Processes the next output samples from the given |timestamp|.
  //
  // @param timestamp Start sample to process.
  // @param num_samples Number of samples to process.
  // @param output Pointer to output samples to be written into.
  void Process(int timestamp, int num_samples, float* output);

  // Resets the performer.
  void Reset();

 private:
  // Instrument to be performed.
  Instrument* instrument_;  // not owned.

  // Message queue to schedule performance directives.
  MessageQueue message_queue_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
