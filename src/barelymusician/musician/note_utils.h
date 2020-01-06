#ifndef BARELYMUSICIAN_MUSICIAN_NOTE_UTILS_H_
#define BARELYMUSICIAN_MUSICIAN_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/musician/note.h"

namespace barelyapi {

// Compares the given two messages with respect to their offset beats.
//
// @param lhs First note.
// @param rhs Second note.
// @return True if the first note comes prior to the second note.
bool CompareNote(const Note& lhs, const Note& rhs);

// Compares the given |note| against the given |offset_beats|.
//
// @param note Note.
// @param offset_beats Offset in beats.
// @return True if the note comes prior to the offset.
bool CompareOffsetBeats(const Note& note, float offset_beats);

// Returns the quantized beat for the given |step| and |num_steps_per_beat|.
//
// @param step Quantized step.
// @param num_steps_per_beat Number of steps per beat.
float GetBeat(int step, int num_steps_per_beat);

// Returns the quantized note index for the given |scale| and |scale_index|.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param scale_index Scale index.
float GetNoteIndex(const std::vector<float>& scale, float scale_index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_NOTE_UTILS_H_
