#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_

#include <vector>

namespace barelyapi {

// Returns the quantized beat duration for the given |num_notes|.
//
// @param num_notes Number of notes.
// @param num_notes_per_beat Number of notes per beat.
float GetBeatDuration(int num_notes, int num_notes_per_beat);

// Returns the quantized note index for the given |scale_index|.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param scale_index Scale index.
float GetNoteIndex(const std::vector<float>& scale, float scale_index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
