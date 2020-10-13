#ifndef BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_

#include <vector>

namespace barelyapi {

// Returns the quantized beat for the given number of beat steps.
//
// @param step Quantized step.
// @param num_steps_per_beat Number of steps per beat.
// @param Beat.
double GetBeat(int step, int num_steps_per_beat);

// Returns the quantized note index for the given scale and index.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param scale_index Scale index.
// @return Note index.
float GetNoteIndex(const std::vector<float>& scale, float scale_index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
