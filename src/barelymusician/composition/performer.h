#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include "barelymusician/composition/message_queue.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

class Performer {
 public:
  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(Instrument* instrument);

  // Performs the given |message|.
  //
  // @param message Message to perform.
  // TODO(#28): Should |Message| be exposed here as a struct?
  void Perform(const Message& message);

  // TODO(#20): Create and pass an AudioBuffer here instead?
  // Processes the next output samples for the given timestamp.
  //
  // @param timestamp Start sample to process.
  // @param num_samples Number of samples to process.
  // @param output Pointer to output samples to be written into.
  void Process(int timestamp, int num_samples, float* output);

  // Resets the performer.
  void Reset();

 private:
  Instrument* instrument_;  // not owned.

  MessageQueue message_queue_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
