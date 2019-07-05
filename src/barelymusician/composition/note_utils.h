#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_

#include <vector>

namespace barelyapi {

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

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
