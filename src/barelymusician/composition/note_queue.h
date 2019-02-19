#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_QUEUE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_QUEUE_H_

#include <list>
#include <vector>

#include "barelymusician/base/note.h"

namespace barelyapi {

// Note queue that receives and dispatches notes according to their timestamps.
class NoteQueue {
 public:
  // Pops the notes within the range from the queue.
  //
  // @param start_sample Start sample to begin.
  // @param num_samples Number of samples to end.
  // @return List of notes within the range.
  std::vector<Note> Pop(int start_sample, int num_samples);

  // Pushes a new note into the queue.
  //
  // @param note Note.
  void Push(const Note& note);

  // Resets the queue.
  void Reset();

 private:
  // Ordered note queue.
  std::list<Note> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_QUEUE_H_
