#ifndef BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Returns the raw note index for the given scale and quantized note index.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param note_index Quantized note index.
// @return Raw note index.
float GetRawNoteIndex(const std::vector<float>& scale,
                      const QuantizedNoteIndex& note_index);

// Returns quantized position for the given number of beat steps.
//
// @param step Quantized step.
// @param num_steps Number of steps per beat.
// @return Raw position.
double GetPosition(int step, int num_steps);

// Returns quantized position.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_