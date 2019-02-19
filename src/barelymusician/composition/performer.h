#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include "barelymusician/composition/note_queue.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

class Performer {
 public:
  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(Instrument* instrument);

  // TODO(#18): Refactor needed here to represent a note?
  // Performs a new note with the given note properties.
  //
  // @param is_on True if the note is turned on.
  // @param index Note index.
  // @param intensity Note intensity.
  // @param timestamp Note timestamp in samples.
  void Perform(bool is_on, float index, float intensity, int timestamp);

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

  NoteQueue note_queue_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
